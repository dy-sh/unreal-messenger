// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileTransferSubsystem.h"

#include "ChatSubsystem.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpServer.h"
#include "Components/ChatComponent.h"
#include "Components/FileTransferComponent.h"
#include "Protocol/ClientServerMessage.h"
#include "Protocol/DownloadFileRequest.h"
#include "Protocol/DownloadFileResponse.h"
#include "Protocol/UploadFileRequest.h"
#include "Protocol/UploadFileResponse.h"
#include "Room/ChatRoom.h"


void UFileTransferSubsystem::StartServer(const int32 Port)
{
	checkf(GetWorld()->GetAuthGameMode(), TEXT("Call StartServer only on the server"));

	if (ConnectionHandler)
	{
		StopServer();
	}

	ChatSubsystem = GetWorld()->GetSubsystem<UChatSubsystem>();
	check(ChatSubsystem);

	ServerPort = Port;

	Server = NewObject<UConnectionTcpServer>();
	Server->Initialize(ServerPort);

	ConnectionHandler = NewObject<UConnectionHandler>();
	ConnectionHandler->OnConnected.AddDynamic(this, &ThisClass::OnConnected);
	ConnectionHandler->OnDisconnected.AddDynamic(this, &ThisClass::OnDisconnected);
	ConnectionHandler->OnReceivedData.AddDynamic(this, &ThisClass::OnReceivedData);
	ConnectionHandler->Open(Server);
}


void UFileTransferSubsystem::StopServer()
{
	if (ConnectionHandler)
	{
		ConnectionHandler->Close();
		ConnectionHandler->OnConnected.RemoveAll(this);
		ConnectionHandler->OnDisconnected.RemoveAll(this);
		ConnectionHandler->OnReceivedData.RemoveAll(this);
		ConnectionHandler = nullptr;
	}
}


void UFileTransferSubsystem::OnConnected(UConnectionBase* Connection)
{
}


void UFileTransferSubsystem::OnDisconnected(UConnectionBase* Connection)
{
}


void UFileTransferSubsystem::OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	UE_LOG(LogTemp, Warning, L"Received %i", ByteArray.Num());

	EClientServerMessageType MessType = ParseMessageType(ByteArray);
	switch (MessType)
	{
		case EClientServerMessageType::UploadFileRequest: ReceiveUploadFileRequest(Connection, ByteArray);
			break;
		case EClientServerMessageType::DownloadFileRequest: ReceiveDownloadFileRequest(Connection, ByteArray);
			break;
		default: break;
	}
}


EClientServerMessageType UFileTransferSubsystem::ParseMessageType(const TArray<uint8>& ByteArray) const
{
	if (ByteArray.Num() < 1) return EClientServerMessageType::Unknown;
	uint8 MessageType;
	int32 Offset = 0;
	UClientServerMessage::ReadPayload(ByteArray, MessageType, Offset);
	return (EClientServerMessageType) MessageType;
}


void UFileTransferSubsystem::ReceiveUploadFileRequest(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	FUploadFileRequestPayload Request = UUploadFileRequest::ParseUploadFileRequestPayload(ByteArray);

	FUploadFileResponsePayload Response;
	Response.FileId = Request.FileName;
	Response.bSuccess = false;

	// check server waiting this fileid
	bool bIsServerWaitingThisFile = false;
	if (auto* Room = ChatSubsystem->GetRoom(Request.RoomId))
	{
		TArray<UFileTransferComponent*> FileComps = Room->GetActiveFileTransferComponentsOfUser(Request.UserId);

		bool bFound = FileComps.ContainsByPredicate([&](const UFileTransferComponent* Comp)
		{
			return Comp &&
			       Comp->GetState() == ETransferringFileState::Uploading &&
			       Comp->GetProceedingFileInfo().FileId == Request.FileId;
		});

		bIsServerWaitingThisFile = bFound;
	}

	if (bIsServerWaitingThisFile)
	{
		// save file
		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
		const FString SavedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
		FString FilePath = SavedPath + Request.FileName;
		FilePath = GetNotExistFileName(FilePath);

		if (FFileHelper::SaveArrayToFile(Request.FileContent, *FilePath))
		{
			Response.bSuccess = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save file %s"), *FilePath);
		}

		// send message to all users
		FTransferredFileInfo FileInfo;
		FileInfo.RoomId = Request.RoomId;
		FileInfo.UserId = Request.UserId;
		FileInfo.UserName = Request.UserName;
		FileInfo.FileId = Request.FileId;
		FileInfo.FileName = Request.FileName;
		FileInfo.FilePath = FilePath;
		FileInfo.Date = FDateTime::Now();
		FileInfo.State = ETransferringFileState::None;

		SendFileInfoToAllUsersInRoom(FileInfo, false);
	}else
	{
		UE_LOG(LogTemp,Error,TEXT("Received a file that the server does not expect from this user."));
	}

	const auto* Message = UUploadFileResponse::CreateUploadFileResponse(Response);
	Connection->Send(Message->GetByteArray());
}


void UFileTransferSubsystem::ReceiveDownloadFileRequest(UConnectionBase* Connection, const TArray<uint8> ByteArray)
{
	FDownloadFileRequestPayload Request = UDownloadFileRequest::ParseDownloadFileRequestPayload(ByteArray);

	FDownloadFileResponsePayload Response;
	Response.FileId = Request.FileId;
	Response.bSuccess = false;

	if (auto* Room = ChatSubsystem->GetRoom(Request.RoomId))
	{
		FTransferredFileInfo FileInfo;
		if (Room->GetFileInfo(Request.FileId, FileInfo))
		{
			Response.FileName = FileInfo.FileName;
			Response.RoomId = FileInfo.RoomId;
			Response.UserId = FileInfo.UserId;
			Response.FileId = FileInfo.FileId;

			IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
			if (FileManager.FileExists(*FileInfo.FilePath))
			{
				if (FFileHelper::LoadFileToArray(Response.FileContent, *FileInfo.FilePath))
				{
					Response.bSuccess = true;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *FileInfo.FilePath);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("File not found: %s"), *FileInfo.FilePath);
			}
		}
	}

	const auto* Message = UDownloadFileResponse::CreateDownloadFileResponse(Response);
	Connection->Send(Message->GetByteArray());
}


void UFileTransferSubsystem::SendFileInfoToAllUsersInRoom(FTransferredFileInfo FileInfo, const bool SendEncrypted)
{
	if (!ChatSubsystem) return;

	auto* Room = ChatSubsystem->GetRoom(FileInfo.RoomId);
	if (!Room) return;

	Room->AddFileInfo(FileInfo);

	for (auto* ChatComponent : Room->GetActiveChatComponents())
	{
		if (SendEncrypted)
		{
			// if (!ChatComponent->GetAuthorizationComponent()) return;
			//
			// const FEncryptionKeys Keys = ChatComponent->GetAuthorizationComponent()->GetClientEncryptionKeys();
			//
			// FChatMessage EncryptedMessage = Message;
			// FString EncryptedData;
			// int32 PayloadSize;
			// if (!UOpenSSLEncryptionLibrary::EncryptAes(Message.Text, Keys.AesKey, Keys.AesIvec, EncryptedData,
			// 	PayloadSize))
			// {
			// 	UE_LOG(LogChatComponent, Error, TEXT("Faild to encrypt server message"))
			// 	return;
			// }
			//
			// EncryptedMessage.Text = EncryptedData;
			// ChatComponent->ClientReceiveEncryptedFileInfo(EncryptedMessage, PayloadSize);
		}
		else
		{
			ChatComponent->ClientReceiveFileInfo(FileInfo);
		}
	}
}


FString UFileTransferSubsystem::GetNotExistFileName(const FString& FilePath) const
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*FilePath)) return FilePath;

	for (int32 i = 1; i < 99999; ++i)
	{
		FString NewFilePath = FPaths::Combine(
			*FPaths::GetPath(FilePath),
			FString::Printf(TEXT("%s(%i).%s"),
				*FPaths::GetBaseFilename(FilePath), i, *FPaths::GetExtension(FilePath)));
		if (!FileManager.FileExists(*NewFilePath))
		{
			return NewFilePath;
		}
	}

	return FString{};
}
