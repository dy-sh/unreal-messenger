// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileTransferComponent.h"
#include "ConnectionHandler.h"
#include "ConnectionTcpClient.h"


void UFileTransferComponent::BeginPlay()
{
	Super::BeginPlay();
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
	ConnectionHandler->OnConnected.AddDynamic(this, %ThisClass::OnConnected);
	ConnectionHandler->OnDisconnected.AddDynamic(this, %ThisClass::OnDisconnected);
	ConnectionHandler->OnReceivedData.AddDynamic(this, %ThisClass::OnReceivedData);
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


void UFileTransferComponent::OnConnected(UConnectionBase* Connection)
{
}


void UFileTransferComponent::OnDisconnected(UConnectionBase* Connection)
{
}


void UFileTransferComponent::OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
}
