// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionTcpClient.h"
#include "Common/TcpSocketBuilder.h"
#include "Worker.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "NetworkUtils.h"


DEFINE_LOG_CATEGORY_STATIC(LogTcpClient, All, All);

UConnectionTcpClient* UConnectionTcpClient::CreateTcpClient(const FString& IpAddress , const int32 Port , const bool bRetry, const bool bAutoReconnect)
{
	auto* SocketTcpClient = NewObject<UConnectionTcpClient>();
	SocketTcpClient->Initialize(IpAddress, Port, bRetry, bAutoReconnect);
	return SocketTcpClient;
}

void UConnectionTcpClient::Initialize(const FString& IpAddress, const int32 Port , const bool bRetry, const bool bAutoReconnect)
{
	ServerIpAddress = IpAddress;
	ConnectionPort = Port;
	bRetryToConnect = bRetry;
	bAutoReconnectIfDisconnected = bAutoReconnect;
}

void UConnectionTcpClient::Open()
{
	if (Socket)
	{
		Close();
	}
	
	if (!UNetworkUtils::GetEndpoint(ServerIpAddress, ConnectionPort, ServerEndpoint)) return;

	Socket = FTcpSocketBuilder(TEXT("NetworkSockets TcpClient"))
		.AsBlocking()
		.WithReceiveBufferSize(ReceiveBufferSize)
		.WithSendBufferSize(SendBufferSize)
		.Build();

	if (!Socket) return;

	Worker = new FWorker([this] { return TryToConnect(); }, 1.0f);
	ConnectionThread = FRunnableThread::Create(Worker, TEXT("NetworkSockets TcpClient Thread"));
}



void UConnectionTcpClient::Close()
{
	Super::Close();

	if (ConnectionThread)
	{
		ConnectionThread->Kill(true);
		delete ConnectionThread;
		ConnectionThread = nullptr;
	}

	if (Worker)
	{
		delete Worker;
		Worker = nullptr;
	}
}


EWorkerResult UConnectionTcpClient::TryToConnect()
{
	if (Socket->Connect(ServerEndpoint.ToInternetAddr().Get()))
	{
		UE_LOG(LogTcpClient, Display, TEXT("TCP Client: connected to %s:%i"),*ServerIpAddress,ConnectionPort);

		OnSocketConnected(this); // todo move it!!!
		StartPolling(Socket);		
	}
	else
	{
		UE_LOG(LogTcpClient, Error, TEXT("TCP Client: faild to connect to %s:%i"),*ServerIpAddress,ConnectionPort);
		
		if (bRetryToConnect)
		{
			return EWorkerResult::InProgress;
		}
	}
	
	return EWorkerResult::Completed;
}

void UConnectionTcpClient::OnSocketDisconnected(UConnectionBase* Connection)
{
	UE_LOG(LogTcpClient, Display, TEXT("TCP Client: (was connected to %s:%i) disconnected"),*ServerIpAddress,ConnectionPort);

	Super::OnSocketDisconnected(Connection);

	if (bAutoReconnectIfDisconnected)
	{
		Open();
	}
}