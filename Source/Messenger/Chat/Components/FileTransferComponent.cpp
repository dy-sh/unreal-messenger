// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileTransferComponent.h"

#include "ChatComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpClient.h"
#include "FileTransferServerComponent.h"
#include "GameFramework/GameModeBase.h"
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
	if (State == EFileTransferringState::SendingFile)
	{
		// if (FileToSend.FileContent.Num() == 0)
		// {
		// 	CloseConnection();
		// 	return;
		// }

		const auto* Message = UUploadFileRequest::CreateUploadFileRequest(1, FileToSend);
		ConnectionHandler->Send(Message->GetByteArray());
		// FileToSend = FFileDataPackageInfo{};
		// CloseConnection();
	}
	else if (State == EFileTransferringState::DownloadingFile)
	{
		ServerGetFile(FileToDownload);
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
}


void UFileTransferComponent::ServerGetFile_Implementation(const FTransferredFileInfo& FileInfo)
{
	if (FileTransferServerComponent)
	{
		FileTransferServerComponent->SendFileToClient(FileInfo);
	}
}
