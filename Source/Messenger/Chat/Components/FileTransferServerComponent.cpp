// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "FileTransferServerComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpServer.h"
#include "FileDataPackage.h"
#include "FileUtilsLibrary.h"


void UFileTransferServerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;

	StartServer();
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

	FFileDataPackageInfo FileReceived;
	FileDataPackage::DataPackageToFile(ByteArray, FileReceived);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	const FString SavedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FString FilePath = SavedPath + FileReceived.FileName;
	FilePath = GetNotExistFileName(FilePath);

	if (!FFileHelper::SaveArrayToFile(FileReceived.FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save file %s"), *FilePath);
	}

	UE_LOG(LogTemp, Warning, L"FileName: %s", *FileReceived.FileName);
	UE_LOG(LogTemp, Warning, L"RoomId: %s", *FileReceived.RoomId);
	UE_LOG(LogTemp, Warning, L"FileSize: %i", FileReceived.FileContent.Num());
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
