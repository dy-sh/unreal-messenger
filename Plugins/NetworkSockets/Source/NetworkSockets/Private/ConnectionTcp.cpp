// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionTcp.h"
#include "Common/TcpSocketBuilder.h"
#include "DataPackage.h"
#include "Worker.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


void UConnectionTcp::StartPolling(FSocket* ConnectionSocket)
{
	bIsClosingSocket = false;

	Socket = ConnectionSocket;
	DataPackage.Clear();

	Worker = new FWorker([this]
	{
		FScopeLock ScopeLock(&Lock);

		return ReceivedData();
	});
	ConnectionThread = FRunnableThread::Create(Worker, TEXT("NetworkSockets TcpConnection Thread"));
}


EWorkerResult UConnectionTcp::ReceivedData()
{
	if (!Socket)
	{
		OnSocketDisconnected(this);
		return EWorkerResult::Completed;
	}

	// checks if the socket is open
	int32 BytesRead = 0;
	uint8 Byte;
	if (!Socket->Recv(&Byte, 1, BytesRead, ESocketReceiveFlags::Peek))
	{
		if (!bIsClosingSocket)
		{
			CloseSocket();
			OnSocketDisconnected(this);
		}
		return EWorkerResult::Completed;
	}

	// waiting for data
	if (!Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(0.1)))
	{
		if (Socket->GetConnectionState() == SCS_ConnectionError)
		{
			if (!bIsClosingSocket)
			{
				CloseSocket();
				OnSocketDisconnected(this);
			}
			return EWorkerResult::Completed;
		}
		return EWorkerResult::InProgress;
	}

	uint32 PendingDataSize = 0;
	while (Socket && Socket->HasPendingData(PendingDataSize))
	{
		// todo: allocate size for all data specified in package header
		// reuse allocated memory for new package
		// decrease allocated memory if package was too big

		uint8* Ptr = DataPackage.AddByteArraySize(PendingDataSize);

		BytesRead = 0;
		if (!Socket->Recv(Ptr, PendingDataSize, BytesRead, ESocketReceiveFlags::WaitAll))
		{
			if (!bIsClosingSocket)
			{
				CloseSocket();
				OnSocketDisconnected(this);
			}
			return EWorkerResult::Completed;
		}

		while (DataPackage.HasReceivedPayload())
		{
			TArray<uint8> ByteArray;

			if (DataPackage.DequeueReceivedPayload(ByteArray))
			{
				OnSocketReceivedData(this, ByteArray);
			}
		}
	}

	return EWorkerResult::InProgress;
}


void UConnectionTcp::Close()
{
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

	Super::Close();
}
