// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionUdpServer.h"
#include "ConnectionUdp.h"
#include "DataPackage.h"
#include "Worker.h"
#include "Common/UdpSocketBuilder.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

DEFINE_LOG_CATEGORY_STATIC(LogUdpServer, All, All);


UConnectionUdpServer* UConnectionUdpServer::CreateUdpServer(const int32 Port)
{
	auto* SocketUdpReceiver = NewObject<UConnectionUdpServer>();
	SocketUdpReceiver->Initialize(Port);
	return SocketUdpReceiver;
}


void UConnectionUdpServer::Initialize(const int32 Port)
{
	ConnectionPort = Port;
}


void UConnectionUdpServer::Open()
{
	if (Socket)
	{
		Close();
	}

	bIsClosingSocket = false;

	Socket = FUdpSocketBuilder(TEXT("NetworkSockets UdpServer"))
	         .WithReceiveBufferSize(ReceiveBufferSize)
	         .BoundToPort(ConnectionPort)
	         .Build();

	if (!Socket) return;

	UE_LOG(LogUdpServer, Display, TEXT("UDP Server: listening on port %i"), ConnectionPort);

	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	Worker = new FWorker([this]
	{
		FScopeLock ScopeLock(&Lock);
		return ReceiveData();
	});
	ConnectionThread = FRunnableThread::Create(Worker, TEXT("NetworkSockets UdpServer Thread"));
}


void UConnectionUdpServer::Close()
{
	DataPackage.Clear();
	OpenedConnections.Reset();

	bIsClosingSocket = true;

	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}

	FScopeLock ScopeLock(&Lock);

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


int32 UConnectionUdpServer::Send(const TArray<uint8>& ByteArray)
{
	UE_LOG(LogUdpServer, Error, TEXT(
		"Sending data from a UDP server to a client is not supported. Use TCP for two way communication."));
	
	return 0;
}


int32 UConnectionUdpServer::SendTo(const TArray<uint8>& ByteArray, const FIPv4Endpoint& Target)
{
	return Send(ByteArray);
}


void UConnectionUdpServer::OnSocketDisconnected(UConnectionBase* Connection)
{
	UE_LOG(LogUdpServer, Display, TEXT("UDP Server: port %i closed"), ConnectionPort);

	Super::OnSocketDisconnected(Connection);
}


EWorkerResult UConnectionUdpServer::ReceiveData()
{
	if (!Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromMilliseconds(10)))
	{
		return EWorkerResult::InProgress;
	}

	uint32 PendingDataSize = 0;
	while (Socket && Socket->HasPendingData(PendingDataSize))
	{
		DataPackage.SetByteArraySize(PendingDataSize);

		int32 BytesRead = 0;
		TSharedRef<FInternetAddr> InternetAddr = SocketSubsystem->CreateInternetAddr();

		if (!Socket->RecvFrom(DataPackage.GetByteArrayPtr(), DataPackage.GetByteArraySize(), BytesRead, *InternetAddr))
		{
			if (!bIsClosingSocket)
			{
				CloseSocket();
				OnSocketDisconnected(this);
			}
			return EWorkerResult::Completed;
		}

		DataPackage.SetByteArraySize(BytesRead);

		auto Endpoint = FIPv4Endpoint(InternetAddr);

		if (!OpenedConnections.Contains(Endpoint))
		{
			auto* Connection = NewObject<UConnectionUdp>(this);
			Connection->Initialize(Endpoint);
			Connection->OnReceivedData.AddDynamic(this, &ThisClass::OnSocketReceivedData);
			OpenedConnections.Add(Endpoint, Connection);

			UE_LOG(LogUdpServer, Display, TEXT("UDP Server: client %s connected"), *Connection->GetIpAddress());

			OnSocketConnected(Connection);
		}

		FScopeLock ScopeLock(&Lock);

		OpenedConnections[Endpoint]->ReceiveData(DataPackage.GetByteArrayPtr(), DataPackage.GetByteArraySize());
		DataPackage.Clear();
	}

	return EWorkerResult::InProgress;
}
