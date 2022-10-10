// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileTransferComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpClient.h"
#include "FileTransferServerComponent.h"
#include "GameFramework/GameModeBase.h"


void UFileTransferComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;
	if (!GetOwner()) return;

	if (const auto* GameMode = GetWorld()->GetAuthGameMode())
	{
		FileTransferServerComponent = GameMode->FindComponentByClass<UFileTransferServerComponent>();
	}

	if (const auto* GameInstance = GetWorld()->GetGameInstance())
	{
		OnlineSessionsSubsystem = GameInstance->GetSubsystem<UOnlineSessionsSubsystem>();
	}
}


UFileTransferComponent::UFileTransferComponent()
{
	SetIsReplicatedByDefault(true);
}


bool UFileTransferComponent::ConnectToServer(const FString& IpAddress, const int32 Port)
{
	if (ConnectionHandler)
	{
		CloseConnection();
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
	if (ConnectionHandler)
	{
		ConnectionHandler->Close();
		ConnectionHandler->OnConnected.RemoveAll(this);
		ConnectionHandler->OnDisconnected.RemoveAll(this);
		ConnectionHandler->OnReceivedData.RemoveAll(this);
		ConnectionHandler = nullptr;
	}
}


bool UFileTransferComponent::SendFile(const FString& RoomId, const FString& FilePath)
{
	if (!OnlineSessionsSubsystem) return false;
	if (IsSendingFile()) return false;

	// todo: call received file function directly if listen server
	const FString ServerIpAddress = OnlineSessionsSubsystem->GetServerIp();
	if (ServerIpAddress.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get server IP address"), *FilePath);
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

	ConnectToServer(ServerIpAddress, ServerPort);

	return true;
}


bool UFileTransferComponent::IsSendingFile() const
{
	return FileToSend.FileContent.Num() > 0;
}


void UFileTransferComponent::ServerRequestFileTransferring_Implementation(const FString& FileName, const int32 FileSize,
	const FString& RoomId)
{
}


void UFileTransferComponent::ClientResponseFileTransferring_Implementation(const FString& FileTransferId)
{
}


void UFileTransferComponent::OnConnected(UConnectionBase* Connection)
{
	if (FileToSend.FileContent.Num() == 0)
	{
		CloseConnection();
		return;
	}

	TArray<uint8> Package;
	FileDataPackage::FileToDataPackage(FileToSend, Package);
	ConnectionHandler->Send(Package);
}


void UFileTransferComponent::OnDisconnected(UConnectionBase* Connection)
{
}


void UFileTransferComponent::OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
}
