// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionTcpServer.h"
#include "ConnectionTcp.h"
#include "DataPackage.h"
#include "Worker.h"
#include "Common/TcpSocketBuilder.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

DEFINE_LOG_CATEGORY_STATIC(LogTcpServer, All, All);

UConnectionTcpServer* UConnectionTcpServer::CreateTcpServer(const int32 Port)
{
	auto* SocketTcpServer = NewObject<UConnectionTcpServer>();
	SocketTcpServer->Initialize(Port);
	return SocketTcpServer;
}


void UConnectionTcpServer::Initialize(const int32 Port)
{
	ConnectionPort = Port;
}


void UConnectionTcpServer::Open()
{
	if(Socket)
	{
		Close();
	}

	Socket = FTcpSocketBuilder(TEXT("NetworkSockets TcpServer"))
	         .AsBlocking()
	         .BoundToPort(ConnectionPort)
	         .Listening(MaxBacklog)
	         .Build();

	if (!Socket) return;

	UE_LOG(LogTcpServer, Display, TEXT("TCP Server: listening on port %i"),ConnectionPort);

	Worker = new FWorker([this] { return ListenPort(); });
	ConnectionThread = FRunnableThread::Create(Worker, TEXT("NetworkSockets TcpServer Thread"));
}


void UConnectionTcpServer::Close()
{
	for (const auto Connection : OpenedConnections)
	{
		Connection->OnDisconnected.RemoveAll(this);
		Connection->OnReceivedData.RemoveAll(this);
		Connection->Close();
	}

	OpenedConnections.Reset();

	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}

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


int32 UConnectionTcpServer::Send(const TArray<uint8>& ByteArray)
{
	int32 TotalBytesSent = 0;
	for (const auto Connection : OpenedConnections)
	{
		TotalBytesSent += Connection->Send(ByteArray);
	}
	return TotalBytesSent;
}


EWorkerResult UConnectionTcpServer::ListenPort()
{
	if (!Socket) return EWorkerResult::Completed;

	const TSharedRef<FInternetAddr> RemoteAddress =
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bHasPendingConnection;
	if (Socket->HasPendingConnection(bHasPendingConnection))
	{
		if (FSocket* NewSocket = Socket->Accept(*RemoteAddress, TEXT("NetworkSockets TcpServer Socket")))
		{
			int32 NewReceiveBufferSize = 0;
			NewSocket->SetReceiveBufferSize(ReceiveBufferSize, NewReceiveBufferSize);
			int32 NewSendBufferSize = 0;
			NewSocket->SetSendBufferSize(SendBufferSize, NewSendBufferSize);

			auto* Connection = NewObject<UConnectionTcp>();
			Connection->OnDisconnected.AddDynamic(this, &ThisClass::OnSocketDisconnected);
			Connection->OnReceivedData.AddDynamic(this, &ThisClass::OnSocketReceivedData);
			Connection->StartPolling(NewSocket);
			OpenedConnections.Add(Connection);

			UE_LOG(LogTcpServer, Display, TEXT("TCP Server: client %s connected"),*Connection->GetIpAddress());

			OnSocketConnected(Connection);
		}
	}

	return EWorkerResult::InProgress;
}


void UConnectionTcpServer::OnSocketDisconnected(UConnectionBase* Connection)
{
	if (auto* Con = Cast<UConnectionTcp>(Connection))
	{
		const int32 Index = OpenedConnections.Find(Con);
		if (Index != INDEX_NONE)
		{
			Con->OnDisconnected.RemoveAll(this);
			Con->OnReceivedData.RemoveAll(this);

			OpenedConnections.RemoveAt(Index);

			OnSocketDisconnected(Connection);
		}
	}

	UE_LOG(LogTcpServer, Display, TEXT("TCP Server: port %i closed"),ConnectionPort);

	Super::OnSocketDisconnected(Connection);
}
