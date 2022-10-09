// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "FileTransferServerComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpServer.h"
#include "FileDataPackage.h"


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
	UE_LOG(LogTemp,Warning,L"Received %i",ByteArray.Num());

	FFileDataPackageInfo FileReceived;
	FileDataPackage::DataPackageToFile(ByteArray, FileReceived);
	
	UE_LOG(LogTemp,Warning,L"FileName: %s",*FileReceived.FileName);
	UE_LOG(LogTemp,Warning,L"RoomId: %s",*FileReceived.RoomId);
	UE_LOG(LogTemp,Warning,L"FileSize: %i",FileReceived.FileContent.Num());
}
