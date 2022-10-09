// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionHandler.h"
#include "Async/Async.h"
#include "ConnectionBase.h"


UConnectionHandler* UConnectionHandler::CreateConnectionHandler()
{
	auto* ConnectionHandler = NewObject<UConnectionHandler>();
	return ConnectionHandler;
}


void UConnectionHandler::Open(UConnectionBase* Connection)
{
	if (!Connection) return;

	Connections.Empty();

	HandledConnection = Connection;
	HandledConnection->OnConnected.AddDynamic(this, &ThisClass::OnSocketConnected);
	HandledConnection->OnDisconnected.AddDynamic(this, &ThisClass::OnSocketDisconnected);
	HandledConnection->OnReceivedData.AddDynamic(this, &ThisClass::OnSocketReceivedData);
	HandledConnection->Open();
}


void UConnectionHandler::Close()
{
	if (!HandledConnection) return;

	HandledConnection->OnConnected.RemoveAll(this);
	HandledConnection->OnDisconnected.RemoveAll(this);
	HandledConnection->OnReceivedData.RemoveAll(this);
	HandledConnection->Close();

	HandledConnection = nullptr;
}


void UConnectionHandler::Send(const TArray<uint8>& ByteArray)
{
	if (bIsDestroying) return;
	if (!HandledConnection) return;

	HandledConnection->Send(ByteArray);
}


void UConnectionHandler::SendTo(const TArray<uint8>& ByteArray, UConnectionBase* Target)
{
	if (bIsDestroying) return;
	if (!HandledConnection) return;

	Target->Send(ByteArray);
}


bool UConnectionHandler::IsConnected()
{
	return Connections.Num() > 0;
}


void UConnectionHandler::BeginDestroy()
{
	bIsDestroying = true;

	if (HandledConnection)
	{
		Close();
	}

	Super::BeginDestroy();
}


void UConnectionHandler::ExecuteDelegate(TFunction<void()> Delegate) const
{
	if (bIsDestroying) return;
	if (!IsValid(this)) return;

	if (bUseGameThread)
	{
		AsyncTask(ENamedThreads::GameThread, [this, Delegate]()
		{
			if (bIsDestroying) return;
			Delegate();
		});
	}
	else
	{
		Delegate();
	}
}


void UConnectionHandler::OnSocketConnected(UConnectionBase* Connection)
{
	Connections.Add(Connection);

	ExecuteDelegate([this,Connection]() { OnConnected.Broadcast(Connection); });
}


void UConnectionHandler::OnSocketDisconnected(UConnectionBase* Connection)
{
	if (Connections.Contains(Connection))
	{
		Connections.Remove(Connection);
	}

	ExecuteDelegate([this,Connection]() { OnDisconnected.Broadcast(Connection); });
}


void UConnectionHandler::OnSocketReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	ExecuteDelegate([this,Connection,ByteArray]() { OnReceivedData.Broadcast(Connection, ByteArray); });
}
