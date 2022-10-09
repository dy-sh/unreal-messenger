// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionUdpClient.h"
#include "Common/UdpSocketBuilder.h"
#include "DataPackage.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "NetworkUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogUdpClient, All, All);


UConnectionUdpClient* UConnectionUdpClient::CreateUdpClient(const FString& IpAddress, const int32 Port)
{
	auto* SocketUdpSender = NewObject<UConnectionUdpClient>();
	SocketUdpSender->Initialize(IpAddress, Port);
	return SocketUdpSender;
}


void UConnectionUdpClient::Initialize(const FString& IpAddress, const int32 Port)
{
	ServerIpAddress = IpAddress;
	ConnectionPort = Port;
}


void UConnectionUdpClient::Open()
{
	if (!UNetworkUtils::GetEndpoint(ServerIpAddress, ConnectionPort, ServerEndpoint)) return;

	Socket = FUdpSocketBuilder(TEXT("NetworkSockets UdpClient"))
	         .WithSendBufferSize(SendBufferSize)
	         .Build();

	if (!Socket) return;

	UE_LOG(LogUdpClient, Display, TEXT("UDP Client: connected to %s:%i"), *ServerIpAddress, ConnectionPort);

	OnSocketConnected(this);
}


void UConnectionUdpClient::Close()
{
	CloseSocket();
}


int32 UConnectionUdpClient::Send(const TArray<uint8>& ByteArray)
{
	return SendTo(ByteArray, ServerEndpoint);
}


int32 UConnectionUdpClient::SendTo(const TArray<uint8>& ByteArray, const FIPv4Endpoint& Target)
{
	if (ByteArray.Num() + DataPackage.GetHeaderSize() > SendBufferSize)
	{
		UE_LOG(LogUdpClient, Error, TEXT(
			"UDP Client: failed to send data. Payload size is greater than SendBufferSize. " \
			"Splitting data into parts is not supported, because order of delivery is not guaranteed. " \
			"Use TCP for large data."));
		return 0;
	}
	return Super::SendTo(ByteArray, Target);
}


void UConnectionUdpClient::OnSocketDisconnected(UConnectionBase* Connection)
{
	UE_LOG(LogUdpClient, Display, TEXT("UDP Client: (was connected to %s:%i) disconnected"), *ServerIpAddress,
	       ConnectionPort);

	Super::OnSocketDisconnected(Connection);
}
