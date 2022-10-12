// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileTransferComponent.h"

#include "ChatComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpClient.h"
#include "GameFramework/GameModeBase.h"
#include "Messenger/Chat/FileTransferSubsystem.h"
#include "Messenger/Chat/Protocol/DownloadFileRequest.h"
#include "Messenger/Chat/Protocol/DownloadFileResponse.h"
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


bool UFileTransferComponent::SendFileToServer(const FString& RoomId, const FString& FilePath)
{
	if (!ChatComponent) return false;
	if (ProceedingFileInfo.State == ETransferringFileState::Uploading
	    || ProceedingFileInfo.State == ETransferringFileState::Downloading)
		return false;

	// todo: call received file function directly if listen server

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*ProceedingFileInfo.FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File not found: %s"), *ProceedingFileInfo.FilePath);
		return false;
	}

	if (!FFileHelper::LoadFileToArray(ProceedingFileContent, *ProceedingFileInfo.FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read file %s"), *ProceedingFileInfo.FilePath);
		return false;
	}

	ProceedingFileInfo.Date = FDateTime::Now();
	ProceedingFileInfo.UserId = ChatComponent->GetUserInfo().UserID;
	ProceedingFileInfo.UserName = ChatComponent->GetUserInfo().UserName;
	ProceedingFileInfo.FilePath = FilePath;
	ProceedingFileInfo.FileName = FPaths::GetBaseFilename(FilePath) + "." + FPaths::GetExtension(FilePath);
	ProceedingFileInfo.State = ETransferringFileState::RequestingUpload;

	ServerRequestUploadingFile();
}


void UFileTransferComponent::ServerRequestUploadingFile_Implementation()
{
	ProceedingFileInfo.State = ETransferringFileState::Uploading;
	ProceedingFileInfo.FileId = FGuid::NewGuid().ToString();
	ClientResponseUploadingFile(ProceedingFileInfo.FileId);
}


void UFileTransferComponent::ClientResponseUploadingFile_Implementation(const FString& FileId)
{
	if (!ChatComponent) return;
	if (!OnlineSessionsSubsystem) return;
	if (ProceedingFileInfo.State != ETransferringFileState::RequestingUpload) return;

	ProceedingFileInfo.FileId = FileId;
	ProceedingFileInfo.State = ETransferringFileState::Uploading;
	OnStartUploadingFile.Broadcast(ProceedingFileInfo);

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


bool UFileTransferComponent::DownloadFileFromServer(const FTransferredFileInfo& FileInfo)
{
	if (!OnlineSessionsSubsystem) return false;
	if (ProceedingFileInfo.State == ETransferringFileState::Uploading
	    || ProceedingFileInfo.State == ETransferringFileState::Downloading)
		return false;

	// todo: call received file function directly if listen server

	const FString ServerIpAddress = OnlineSessionsSubsystem->GetServerIp();
	if (ServerIpAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get server IP address"));
		return false;
	}

	ProceedingFileInfo = FileInfo;

	ProceedingFileInfo.State = ETransferringFileState::Downloading;
	ConnectToServer(ServerIpAddress, ServerPort);

	return true;
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
		Payload.RoomId = ProceedingFileInfo.RoomId;
		Payload.UserId = ProceedingFileInfo.UserId;
		Payload.FileName = ProceedingFileInfo.FileName;
		Payload.FileContent = ProceedingFileContent;

		const auto* Message = UUploadFileRequest::CreateUploadFileRequest(Payload);
		ConnectionHandler->Send(Message->GetByteArray());
	}
	else if (ProceedingFileInfo.State == ETransferringFileState::Downloading)
	{
		FDownloadFileRequestPayload Payload;
		Payload.FileId = ProceedingFileInfo.FileId;
		Payload.RoomId = ProceedingFileInfo.RoomId;
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
	ProceedingFileInfo.State = ETransferringFileState::None;
}


void UFileTransferComponent::OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	EClientServerMessageType MessType = ParseMessageType(ByteArray);
	switch (MessType)
	{
		case EClientServerMessageType::DownloadFileResponse: ReceiveDownloadFileResponse(ByteArray);
			break;
		default: break;
	}
}


EClientServerMessageType UFileTransferComponent::ParseMessageType(const TArray<uint8>& ByteArray) const
{
	if (ByteArray.Num() < 1) return EClientServerMessageType::Unknown;
	int32 MessageType;
	int32 Offset = 0;
	UClientServerMessage::ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, MessageType, Offset);
	return (EClientServerMessageType) MessageType;
}


void UFileTransferComponent::ReceiveDownloadFileResponse(const TArray<uint8> ByteArray)
{
	FDownloadFileResponsePayload ReceivedFile = UDownloadFileResponse::ParseDownloadFileResponsePayload(ByteArray);

	// save file
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	const FString SavedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FString FilePath = SavedPath + ReceivedFile.FileName;
	FilePath = GetNotExistFileName(FilePath);

	if (!FFileHelper::SaveArrayToFile(ReceivedFile.FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save file %s"), *FilePath);
		return; // todo: send bad request
	}

	OnDownloadingFileComplete.Broadcast(ProceedingFileInfo, true);
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
