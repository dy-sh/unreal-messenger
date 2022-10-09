// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ConnectionUdp.h"
#include "DataPackage.h"

void UConnectionUdp::Initialize(const FIPv4Endpoint Endpoint)
{
	ConnectionEndpoint = Endpoint;
}


bool UConnectionUdp::GetIpAddressAsByteArray(TArray<uint8>& IpAddress) const
{
	IpAddress.SetNum(4);
	IpAddress[0] = ConnectionEndpoint.Address.A;
	IpAddress[1] = ConnectionEndpoint.Address.B;
	IpAddress[2] = ConnectionEndpoint.Address.C;
	IpAddress[3] = ConnectionEndpoint.Address.D;

	return true;
}


FString UConnectionUdp::GetIpAddress() const
{
	return ConnectionEndpoint.Address.ToString();
}


void UConnectionUdp::ReceiveData(const uint8* ByteArray, const int32 Size)
{
	DataPackage.Add(ByteArray, Size);

	while (DataPackage.HasReceivedPayload())
	{
		TArray<uint8> ReceivedPayload;

		if (DataPackage.DequeueReceivedPayload(ReceivedPayload))
		{
			OnSocketReceivedData(this, ReceivedPayload);
		}
	}
}
