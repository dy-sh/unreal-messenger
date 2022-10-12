// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileTransferComponent.h"

#include "ChatComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpClient.h"
#include "FileTransferServerComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Messenger/Chat/Protocol/DownloadFileRequest.h"
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

	if (const auto* GameMode = GetWorld()->GetAuthGameMode())
	{
		FileTransferServerComponent = GameMode->FindComponentByClass<UFileTransferServerComponent>();
	}

	ChatComponent = GetOwner()->FindComponentByClass<UChatComponent>();

	if (const auto* GameInstance = GetWorld()->GetGameInstance())
	{
		OnlineSessionsSubsystem = GameInstance->GetSubsystem<UOnlineSessionsSubsystem>();
	}
}


bool UFileTransferComponent::ConnectToServer(const FString& IpAddress, const int32 Port)
{
	if (ConnectionHandler)
	{
		const EFileTransferringState TempState = State;
		CloseConnection();
		State = TempState;
	}

	Client = NewObject<UConnectionTcpClient>();
	Client->Initialize(IpAddress, Port);

	ConnectionHandler = NewObject<UConnectionHandler>();
	ConnectionHandler->OnConnected.AddDynamic(this, &ThisClass::OnConnected);
	ConnectionHandler->OnDisconnected.AddDynamic(this, &ThisClass::OnDisconnected);
	ConnectionHandler->OnReceivedData.AddDynamic(this, &ThisClass::OnReceivedData);
	ConnectionHandler->Open(Client);

	return true;
}


void UFileTransferComponent::CloseConnection()
{
	State = EFileTransferringState::None;

	if (ConnectionHandler)
	{
		ConnectionHandler->Close();
		ConnectionHandler->OnConnected.RemoveAll(this);
		ConnectionHandler->OnDisconnected.RemoveAll(this);
		ConnectionHandler->OnReceivedData.RemoveAll(this);
		ConnectionHandler = nullptr;
	}
}


bool UFileTransferComponent::SendFileToServer(const FString& RoomId, const FString& FilePath)
{
	if (!ChatComponent) return false;
	if (!OnlineSessionsSubsystem) return false;
	if (State != EFileTransferringState::None) return false;

	// todo: call received file function directly if listen server
	const FString ServerIpAddress = OnlineSessionsSubsystem->GetServerIp();
	if (ServerIpAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get server IP address"));
		return false;
	}

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File not found: %s"), *FilePath);
		return false;
	}

	if (!FFileHelper::LoadFileToArray(FileToSend.FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read file %s"), *FilePath);
		return false;
	}

	FileToSend.FileName = FPaths::GetBaseFilename(FilePath) + "." + FPaths::GetExtension(FilePath);
	FileToSend.RoomId = RoomId;
	FileToSend.UserId = ChatComponent->GetUserInfo().UserID;

	State = EFileTransferringState::SendingFile;
	ConnectToServer(ServerIpAddress, ServerPort);

	return true;
}


bool UFileTransferComponent::DownloadFileFromServer(const FTransferredFileInfo& FileInfo)
{
	if (!OnlineSessionsSubsystem) return false;
	if (State != EFileTransferringState::None) return false;

	// todo: call received file function directly if listen server
	const FString ServerIpAddress = OnlineSessionsSubsystem->GetServerIp();
	if (ServerIpAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get server IP address"));
		return false;
	}

	FileToDownload = FileInfo;

	State = EFileTransferringState::DownloadingFile;
	ConnectToServer(ServerIpAddress, ServerPort);
	return true;
}


void UFileTransferComponent::OnConnected(UConnectionBase* Connection)
{
	if (!ChatComponent) return;

	if (State == EFileTransferringState::SendingFile)
	{
		const auto* Message = UUploadFileRequest::CreateUploadFileRequest(FileToSend);
		ConnectionHandler->Send(Message->GetByteArray());
		// FileToSend = FFileDataPackageInfo{};
		// CloseConnection();
		State = EFileTransferringState::None;
	}
	else if (State == EFileTransferringState::DownloadingFile)
	{
		FDownloadFileRequestPayload Payload;
		Payload.FileId = FileToDownload.FileId;
		Payload.RoomId = FileToDownload.RoomId;
		Payload.UserId = ChatComponent->GetUserInfo().UserID;
		const auto* Message = UDownloadFileRequest::CreateDownloadFileRequest(Payload);
		ConnectionHandler->Send(Message->GetByteArray());
		State = EFileTransferringState::None;
	}
	else
	{
		CloseConnection();
	}

	
}


void UFileTransferComponent::OnDisconnected(UConnectionBase* Connection)
{
}


void UFileTransferComponent::OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	EClientServerMessageType MessType = ParseMessageType(ByteArray);
	switch (MessType)
	{
		case EClientServerMessageType::DownloadFileResponse: ReceiveDownloadFileResponse(ByteArray);
		break;
		default:
			break;
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