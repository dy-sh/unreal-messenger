// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "FileTransferServerComponent.h"

#include "ChatComponent.h"
#include "ChatServerComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpServer.h"
#include "FileUtilsLibrary.h"
#include "Messenger/Chat/ChatTypes.h"
#include "GameFramework/GameModeBase.h"
#include "Messenger/Authorization/AuthorizationComponent.h"
#include "Messenger/Chat/Protocol/DownloadFileRequest.h"
#include "Messenger/Chat/Protocol/DownloadFileResponse.h"
#include "Messenger/Chat/Protocol/UploadFileRequest.h"
#include "Messenger/Chat/Room/ChatRoom.h"


void UFileTransferServerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;

	StartServer();

	if (const auto* GameMode = GetWorld()->GetAuthGameMode())
	{
		ChatServerComponent = GameMode->FindComponentByClass<UChatServerComponent>();
	}
}


void UFileTransferServerComponent::StartServer()
{
	if (ConnectionHandler)
	{
		StopServer();
	}

	Server = NewObject<UConnectionTcpServer>();
	Server->Initialize(ServerPort);

	ConnectionHandler = NewObject<UConnectionHandler>();
	ConnectionHandler->OnConnected.AddDynamic(this, &ThisClass::OnConnected);
	ConnectionHandler->OnDisconnected.AddDynamic(this, &ThisClass::OnDisconnected);
	ConnectionHandler->OnReceivedData.AddDynamic(this, &ThisClass::OnReceivedData);
	ConnectionHandler->Open(Server);
}


void UFileTransferServerComponent::StopServer()
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


void UFileTransferServerComponent::OnConnected(UConnectionBase* Connection)
{
}


void UFileTransferServerComponent::OnDisconnected(UConnectionBase* Connection)
{
}


void UFileTransferServerComponent::OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	UE_LOG(LogTemp, Warning, L"Received %i", ByteArray.Num());
	
	EClientServerMessageType MessType = ParseMessageType(ByteArray);
	switch (MessType)
	{
		case EClientServerMessageType::UploadFileRequest: ReceiveUploadFileRequest(ByteArray);
			break;
		case EClientServerMessageType::DownloadFileRequest: ReceiveDownloadFileRequest(Connection, ByteArray);
			break;
		default:
			break;
	}
}


EClientServerMessageType UFileTransferServerComponent::ParseMessageType(const TArray<uint8>& ByteArray) const
{
	if (ByteArray.Num() < 1) return EClientServerMessageType::Unknown;
	int32 MessageType;
	int32 Offset = 0;
	UClientServerMessage::ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, MessageType, Offset);
	return (EClientServerMessageType) MessageType;
}


void UFileTransferServerComponent::ReceiveUploadFileRequest(const TArray<uint8>& ByteArray)
{
	FUploadFileRequestPayload ReceivedFile = UUploadFileRequest::ParseUploadFileRequestPayload(ByteArray);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	const FString SavedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FString FilePath = SavedPath + ReceivedFile.FileName;
	FilePath = GetNotExistFileName(FilePath);

	if (!FFileHelper::SaveArrayToFile(ReceivedFile.FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save file %s"), *FilePath);
		return; // todo: send bad request
	}

	UE_LOG(LogTemp, Warning, L"RoomId: %s", *ReceivedFile.RoomId);
	UE_LOG(LogTemp, Warning, L"UserId: %s", *ReceivedFile.UserId);
	UE_LOG(LogTemp, Warning, L"FileName: %s", *ReceivedFile.FileName);
	UE_LOG(LogTemp, Warning, L"FileSize: %i", ReceivedFile.FileContent.Num());

	FTransferredFileInfo FileInfo;
	FileInfo.RoomId = ReceivedFile.RoomId;
	FileInfo.UserId = ReceivedFile.UserId;
	FileInfo.UserName = "User";
	FileInfo.FileId = FGuid::NewGuid().ToString();
	FileInfo.FileName = ReceivedFile.FileName;
	FileInfo.SavedFileName = FilePath;
	FileInfo.Date = FDateTime::Now();

	SendFileInfoToAllUsersInRoom(FileInfo, false);
}


void UFileTransferServerComponent::ReceiveDownloadFileRequest(UConnectionBase* Connection, const TArray<uint8> ByteArray)
{
	FDownloadFileRequestPayload Request = UDownloadFileRequest::ParseDownloadFileRequestPayload(ByteArray);

	auto* Room = ChatServerComponent->GetRoom(Request.RoomId);
	if (!Room) return; // todo: send bad request

	FTransferredFileInfo FileInfo;
	if (!Room->GetFileInfo(Request.FileId, FileInfo)) return; // todo: send bad request

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*FileInfo.SavedFileName))
	{
		UE_LOG(LogTemp, Error, TEXT("File not found: %s"), *FileInfo.SavedFileName);
		return; // todo: send bad request
	}

	FDownloadFileResponsePayload FileToSend;

	if (!FFileHelper::LoadFileToArray(FileToSend.FileContent, *FileInfo.SavedFileName))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *FileInfo.SavedFileName);
		return; // todo: send bad request
	}

	FileToSend.FileName = FileInfo.FileName;
	FileToSend.RoomId = FileInfo.RoomId;
	FileToSend.UserId = FileInfo.UserId;
	FileToSend.FileId = FileInfo.FileId;

	const auto* Message = UDownloadFileResponse::CreateDownloadFileResponse(FileToSend);
	Connection->Send(Message->GetByteArray());
}


void UFileTransferServerComponent::SendFileInfoToAllUsersInRoom(FTransferredFileInfo FileInfo, const bool SendEncrypted)
{
	if (!ChatServerComponent) return;

	auto* Room = ChatServerComponent->GetRoom(FileInfo.RoomId);
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


FString UFileTransferServerComponent::GetNotExistFileName(const FString& FilePath) const
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
