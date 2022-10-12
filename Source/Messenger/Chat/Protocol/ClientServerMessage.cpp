// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientServerMessage.h"
#include "NetworkUtils.h"


void UClientServerMessage::WritePayloadToDataByteArray(const int32 DataSizeBitDepth, const TArray<uint8>& Payload,
	TArray<uint8>& Data, int32& Offset)
{
	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Data[i + Offset] = Payload.Num() >> i * 8;
	}
	Offset += DataSizeBitDepth;

	FMemory::Memcpy(Data.GetData() + Offset, Payload.GetData(), Payload.Num());
	Offset += Payload.Num();
}


void UClientServerMessage::ReadPayloadFromDataByteArray(const int32 DataSizeBitDepth, const TArray<uint8>& Data,
	TArray<uint8>& Payload, int32& Offset)
{
	int32 Size = 0;

	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Size |= Data[i + Offset] << i * 8;
	}
	Offset += DataSizeBitDepth;

	Payload.SetNum(Size);
	FMemory::Memcpy(Payload.GetData(), Data.GetData() + Offset, Size);
	Offset += Size;
}



void UClientServerMessage::WritePayloadToDataByteArray(const int32 DataSizeBitDepth, const int32& Payload,
	TArray<uint8>& Data, int32& Offset)
{
	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Data[i + Offset] = Payload >> i * 8;
	}
	Offset += DataSizeBitDepth;
}


void UClientServerMessage::ReadPayloadFromDataByteArray(const int32 DataSizeBitDepth, const TArray<uint8>& Data,
	int32& Payload, int32& Offset)
{
	Payload = 0;

	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Payload |= Data[i + Offset] << i * 8;
	}
	Offset += DataSizeBitDepth;
}