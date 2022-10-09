// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionBase.h"
#include "Misc/OutputDeviceRedirector.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"


int32 UConnectionBase::Send(const TArray<uint8>& ByteArray)
{
	if (!Socket) return 0;

	int32 TotalBytesSent = 0;
	const TArray<uint8>& SendByteArray = DataPackage.PayloadToDataPackage(ByteArray);

	// split data into parts of BufferSize size
	const int32 Parts = SendByteArray.Num() / SendBufferSize;
	for (int i = 0; i <= Parts; ++i)
	{
		const int32 Offset = i * SendBufferSize;
		const int32 Length = FMath::Min(SendByteArray.Num() - TotalBytesSent, SendBufferSize);
		int32 BytesSent = 0;
		Socket->Send(SendByteArray.GetData() + Offset, Length, BytesSent);
		TotalBytesSent += BytesSent;
	}
	return TotalBytesSent;
}


int32 UConnectionBase::SendTo(const TArray<uint8>& ByteArray, const FIPv4Endpoint& Target)
{
	if (!Socket) return 0;

	int32 TotalBytesSent = 0;
	const TArray<uint8>& SendByteArray = DataPackage.PayloadToDataPackage(ByteArray);

	// split data into parts of BufferSize size
	const int32 Parts = SendByteArray.Num() / SendBufferSize;
	for (int i = 0; i <= Parts; ++i)
	{
		const int32 Offset = i * SendBufferSize;
		const int32 Length = FMath::Min(SendByteArray.Num() - TotalBytesSent, SendBufferSize);
		int32 BytesSent = 0;
		Socket->SendTo(SendByteArray.GetData() + Offset, Length, BytesSent, Target.ToInternetAddr().Get());
		TotalBytesSent += BytesSent;
	}
	return TotalBytesSent;
}


bool UConnectionBase::GetIpAddressAsByteArray(TArray<uint8>& IpAddress) const
{
	if (!Socket) return false;

	const TSharedPtr<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->
		CreateInternetAddr();
	Socket->GetPeerAddress(*InternetAddr);
	IpAddress = InternetAddr->GetRawIp();

	return true;
}


FString UConnectionBase::GetIpAddress() const
{
	if (!Socket) return FString{};

	const TSharedPtr<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->
		CreateInternetAddr();
	Socket->GetPeerAddress(*InternetAddr);
	return InternetAddr->ToString(false);
}


void UConnectionBase::BeginDestroy()
{
	if (Socket)
	{
		Close();
	}

	Super::BeginDestroy();
}


void UConnectionBase::CloseSocket()
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
}


void UConnectionBase::OnSocketConnected(UConnectionBase* Connection)
{
	OnConnected.Broadcast(Connection);
}


void UConnectionBase::OnSocketDisconnected(UConnectionBase* Connection)
{
	OnDisconnected.Broadcast(Connection);
}


void UConnectionBase::OnSocketReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray)
{
	OnReceivedData.Broadcast(Connection, ByteArray);
}
