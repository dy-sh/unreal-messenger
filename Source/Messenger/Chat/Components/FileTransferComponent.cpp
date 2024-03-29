﻿// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileTransferComponent.h"

#include "ChatComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpClient.h"
#include "RoomComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Messenger/Chat/ChatSubsystem.h"
#include "Messenger/Chat/FileTransferSubsystem.h"
#include "Messenger/Chat/Protocol/DownloadFileRequest.h"
#include "Messenger/Chat/Protocol/DownloadFileResponse.h"
#include "Messenger/Chat/Protocol/UploadFileResponse.h"
#include "Messenger/Chat/Room/ChatRoom.h"
#include "OnlineSessions/OnlineSessionsSubsystem.h"


UFileTransferComponent::UFileTransferComponent()
{
	SetIsReplicatedByDefault(true);
}


void UFileTransferComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;
	if (!GetOwner()) return;

	if (GetWorld()->GetAuthGameMode() != nullptr)
	{
		FileTransferSubsystem = GetWorld()->GetSubsystem<UFileTransferSubsystem>();
		if (!FileTransferSubsystem->IsServerStarted())
		{
			FileTransferSubsystem->StartServer();
		}
	}

	ChatComponent = GetOwner()->FindComponentByClass<UChatComponent>();

	if (const auto* GameInstance = GetWorld()->GetGameInstance())
	{
		OnlineSessionsSubsystem = GameInstance->GetSubsystem<UOnlineSessionsSubsystem>();
	}
}


bool UFileTransferComponent::SendFileToServer(const FString& FilePath)
{
	if (!ChatComponent) return false;
	if (!ChatComponent->GetRoomComponent()) return false;
	if (!GetOwner()) return false;
	if (ProceedingFileInfo.State == ETransferringFileState::Uploading
	    || ProceedingFileInfo.State == ETransferringFileState::Downloading)
		return false;

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (!FileManager.FileExists(*FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File not found: %s"), *FilePath);
		return false;
	}

	if (!FFileHelper::LoadFileToArray(ProceedingFileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read file %s"), *FilePath);
		return false;
	}

	ProceedingFileInfo.FilePath = FilePath;
	ProceedingFileInfo.FileName = FPaths::GetBaseFilename(ProceedingFileInfo.FilePath) + "." + FPaths::GetExtension(
		                              ProceedingFileInfo.FilePath);
	ProceedingFileRoomId = ChatComponent->GetRoomComponent()->GetActiveRoomId();


	if (GetOwner()->HasAuthority())
	{
		FUploadFileRequestPayload Request;

		Request.FileId = FGuid::NewGuid().ToString();
		Request.FileName = ProceedingFileInfo.FileName;
		Request.RoomId = ProceedingFileRoomId;
		Request.UserId = ChatComponent->GetUserInfo().UserID;
		Request.UserName = ChatComponent->GetUserInfo().UserName;
		Request.FileContent = ProceedingFileContent;
		const bool bSuccess = FileTransferSubsystem->SaveReceivedFile(Request,ProceedingFileInfo.FilePath);

		ProceedingFileInfo.FileId = Request.FileId;
		ProceedingFileInfo.FileName =  FPaths::GetBaseFilename(ProceedingFileInfo.FilePath) + "." + FPaths::GetExtension(
							   ProceedingFileInfo.FilePath);
		ProceedingFileInfo.State = ETransferringFileState::Uploaded;
		OnUploadingFileComplete.Broadcast(ProceedingFileInfo, bSuccess);
		return bSuccess;
	}
	
	ProceedingFileInfo.State = ETransferringFileState::RequestingUpload;
	ServerRequestUploadingFile();
	return true;
}


void UFileTransferComponent::ServerRequestUploadingFile_Implementation()
{
	ProceedingFileInfo.FileId = FGuid::NewGuid().ToString();
		ProceedingFileInfo.State = ETransferringFileState::Uploading;
	
		ClientResponseUploadingFile(ProceedingFileInfo.FileId);
}


void UFileTransferComponent::ClientResponseUploadingFile_Implementation(const FString& FileId)
{
	if (!ChatComponent) return;
	if (!OnlineSessionsSubsystem) return;
	if (ProceedingFileInfo.State != ETransferringFileState::RequestingUpload) return;

	ProceedingFileInfo.FileId = FileId;
	ProceedingFileInfo.State = ETransferringFileState::Uploading;
	OnUploadingFileStarted.Broadcast(ProceedingFileInfo);

	// todo: call received file function directly if listen server
	const FString ServerIpAddress = OnlineSessionsSubsystem->GetServerIp();
	if (ServerIpAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get server IP address"));
		return;
	}

	ProceedingFileInfo.State = ETransferringFileState::Uploading;
	ConnectToServer(ServerIpAddress, ServerPort);
}


bool UFileTransferComponent::DownloadFileFromServer(const FString& FileId)
{
	if (!OnlineSessionsSubsystem) return false;
	if (FileId.IsEmpty()) return false;
	if (ProceedingFileInfo.State == ETransferringFileState::Uploading
	    || ProceedingFileInfo.State == ETransferringFileState::Downloading)
		return false;

	ProceedingFileInfo.FileId = FileId;
	ProceedingFileRoomId = ChatComponent->GetRoomComponent()->GetActiveRoomId();

	if (GetOwner()->HasAuthority())
	{
		bool bSuccess = false;
		if (const auto* ChatSubsystem = GetWorld()->GetSubsystem<UChatSubsystem>())
		{
			if (auto* Room = ChatSubsystem->GetRoom(ProceedingFileRoomId))
			{
				FTransferredFileInfo FileInfo;
				if (Room->GetFileInfo(FileId, FileInfo))
				{
					IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
					if (FileManager.FileExists(*FileInfo.FilePath))
					{
						ProceedingFileInfo.FilePath = FileInfo.FilePath;
						ProceedingFileInfo.FileName = FileInfo.FileName;
						bSuccess = true;
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Faild to download local file. Does not exist."));
					}
				}
			}
		}
		ProceedingFileInfo.State = ETransferringFileState::Downloaded;
		OnDownloadingFileComplete.Broadcast(ProceedingFileInfo, bSuccess);
		return bSuccess;
	}

	const FString ServerIpAddress = OnlineSessionsSubsystem->GetServerIp();
	if (ServerIpAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get server IP address"));
		return false;
	}

	ProceedingFileInfo.State = ETransferringFileState::Downloading;
	ConnectToServer(ServerIpAddress, ServerPort);

	return true;
}


bool UFileTransferComponent::SaveDownloadedFile(const FTransferredFileInfo& FileInfo, const FString& Path)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (!FileManager.FileExists(*FileInfo.FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Faild to save file. File downloaded but not exist."));
		return false;
	}

	if (FileManager.FileExists(*Path))
	{
		UE_LOG(LogTemp, Error, TEXT("Faild to save file. File already exist."));
		return false;
	}

	return FileManager.CopyFile(*Path, *FileInfo.FilePath);
}


void UFileTransferComponent::ConnectToServer(const FString& IpAddress, const int32 Port)
{
	if (ConnectionHandler)
	{
		const ETransferringFileState TempState = ProceedingFileInfo.State;
		CloseConnection();
		ProceedingFileInfo.State = TempState;
	}

	Client = NewObject<UConnectionTcpClient>();
	Client->Initialize(IpAddress, Port);

	ConnectionHandler = NewObject<UConnectionHandler>();
	ConnectionHandler->OnConnected.AddDynamic(this, &ThisClass::OnConnected);
	ConnectionHandler->OnDisconnected.AddDynamic(this, &ThisClass::OnDisconnected);
	ConnectionHandler->OnReceivedData.AddDynamic(this, &ThisClass::OnReceivedData);
	ConnectionHandler->Open(Client);
}


void UFileTransferComponent::CloseConnection()
{
	ProceedingFileInfo.State = ETransferringFileState::None;

	if (ConnectionHandler)
	{
		ConnectionHandler->Close();
		ConnectionHandler->OnConnected.RemoveAll(this);
		ConnectionHandler->OnDisconnected.RemoveAll(this);
		ConnectionHandler->OnReceivedData.RemoveAll(this);
		ConnectionHandler = nullptr;
	}
}


void UFileTransferComponent::OnConnected(UConnectionBase* Connection)
{
	if (!ChatComponent) return;

	if (ProceedingFileInfo.State == ETransferringFileState::Uploading)
	{
		FUploadFileRequestPayload Payload;
		Payload.RoomId = ProceedingFileRoomId;
		Payload.FileId = ProceedingFileInfo.FileId;
		Payload.UserId = ChatComponent->GetUserInfo().UserID;
		Payload.UserName = ChatComponent->GetUserInfo().UserName;
		Payload.FileName = FPaths::GetBaseFilename(ProceedingFileInfo.FilePath) + "." + FPaths::GetExtension(
			                   ProceedingFileInfo.FilePath);
		Payload.FileContent = ProceedingFileContent;

		const auto* Message = UUploadFileRequest::CreateUploadFileRequest(Payload);
		ConnectionHandler->Send(Message->GetByteArray());
	}
	else if (ProceedingFileInfo.State == ETransferringFileState::Downloading)
	{
		FDownloadFileRequestPayload Payload;
		Payload.RoomId = ProceedingFileRoomId;
		Payload.FileId = ProceedingFileInfo.FileId;
		Payload.UserId = ChatComponent->GetUserInfo().UserID;

		const auto* Message = UDownloadFileRequest::CreateDownloadFileRequest(Payload);
		ConnectionHandler->Send(Message->GetByteArray());
	}
	else
	{
		CloseConnection();
	}
}


void UFileTransferComponent::OnDisconnected(UConnectionBase* Connection)
{
	if (ProceedingFileInfo.State == ETransferringFileState::Uploading)
	{
		OnUploadingFileComplete.Broadcast(ProceedingFileInfo, false);
	}
	else if (ProceedingFileInfo.State == ETransferringFileState::Downloading)
	{
		OnDownloadingFileComplete.Broadcast(ProceedingFileInfo, false);
	}

	ProceedingFileInfo.State = ETransferringFileState::None;
}


void UFileTransferComponent::OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	const EClientServerMessageType MessType = ParseMessageType(ByteArray);
	switch (MessType)
	{
		case EClientServerMessageType::UploadFileResponse: ReceiveUploadFileResponse(ByteArray);
			break;
		case EClientServerMessageType::DownloadFileResponse: ReceiveDownloadFileResponse(ByteArray);
			break;
		default: break;
	}

	CloseConnection();
}


EClientServerMessageType UFileTransferComponent::ParseMessageType(const TArray<uint8>& ByteArray) const
{
	if (ByteArray.Num() < 1) return EClientServerMessageType::Unknown;
	uint8 MessageType;
	int32 Offset = 0;
	UClientServerMessage::ReadPayload(ByteArray, MessageType, Offset);
	return (EClientServerMessageType) MessageType;
}


void UFileTransferComponent::ReceiveUploadFileResponse(const TArray<uint8>& ByteArray)
{
	const FUploadFileResponsePayload Response = UUploadFileResponse::ParseUploadFileResponsePayload(ByteArray);

	ProceedingFileInfo.State = ETransferringFileState::Uploaded;
	OnUploadingFileComplete.Broadcast(ProceedingFileInfo, Response.bSuccess);
}


void UFileTransferComponent::ReceiveDownloadFileResponse(const TArray<uint8>& ByteArray)
{
	const FDownloadFileResponsePayload Response = UDownloadFileResponse::ParseDownloadFileResponsePayload(ByteArray);

	if (Response.bSuccess)
	{
		// save file
		const FString SavedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
		ProceedingFileInfo.FilePath = SavedPath + Response.FileName;
		ProceedingFileInfo.FilePath = GetNotExistFileName(ProceedingFileInfo.FilePath);
		ProceedingFileInfo.FileName = Response.FileName;

		IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
		if (!FFileHelper::SaveArrayToFile(Response.FileContent, *ProceedingFileInfo.FilePath))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save file %s"), *ProceedingFileInfo.FilePath);

			ProceedingFileInfo.State = ETransferringFileState::None;
			OnDownloadingFileComplete.Broadcast(ProceedingFileInfo, false);
		}
	}

	ProceedingFileInfo.State = ETransferringFileState::Downloaded;
	OnDownloadingFileComplete.Broadcast(ProceedingFileInfo, Response.bSuccess);
}


FString UFileTransferComponent::GetNotExistFileName(const FString& FilePath) const
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
