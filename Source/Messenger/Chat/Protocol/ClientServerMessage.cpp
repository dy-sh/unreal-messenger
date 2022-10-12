// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientServerMessage.h"
#include "NetworkUtils.h"

int32 UClientServerMessage::CalculatePayloadSize(const TArray<uint8>& Data)
{
	return BYTE_ARRAY_SIZE_BIT_DEPTH + Data.Num();
}

void UClientServerMessage::WritePayload(const TArray<uint8>& Payload, TArray<uint8>& Data, int32& Offset)
{
	for (uint8 i = 0; i < BYTE_ARRAY_SIZE_BIT_DEPTH; ++i)
	{
		Data[i + Offset] = Payload.Num() >> i * 8;
	}
	Offset += BYTE_ARRAY_SIZE_BIT_DEPTH;

	FMemory::Memcpy(Data.GetData() + Offset, Payload.GetData(), Payload.Num());
	Offset += Payload.Num();
}


void UClientServerMessage::ReadPayload(const TArray<uint8>& Data, TArray<uint8>& Payload, int32& Offset)
{
	int32 Size = 0;

	for (uint8 i = 0; i < BYTE_ARRAY_SIZE_BIT_DEPTH; ++i)
	{
		Size |= Data[i + Offset] << i * 8;
	}
	Offset += BYTE_ARRAY_SIZE_BIT_DEPTH;

	Payload.SetNum(Size);
	if (Size>0)
	{
		FMemory::Memcpy(Payload.GetData(), Data.GetData() + Offset, Size);
	}
	Offset += Size;
}


int32 UClientServerMessage::CalculatePayloadSize(const FString& Payload)
{
	TArray<uint8> ByteArray;
	UNetworkUtils::StringToByteArray(Payload, ByteArray);
	return BYTE_ARRAY_SIZE_BIT_DEPTH + ByteArray.Num();
}


void UClientServerMessage::WritePayload(const FString& Payload, TArray<uint8>& Data, int32& Offset)
{
	TArray<uint8> ByteArray;
	UNetworkUtils::StringToByteArray(Payload, ByteArray);
	WritePayload(ByteArray, Data, Offset);
}


void UClientServerMessage::ReadPayload(const TArray<uint8>& Data, FString& Payload, int32& Offset)
{
	TArray<uint8> FileNameByteArray;
	ReadPayload(Data, FileNameByteArray, Offset);
	Payload = UNetworkUtils::ByteArrayToString(Data);
}



int32 UClientServerMessage::CalculatePayloadSize(const bool Payload)
{
	return 1;
}

void UClientServerMessage::WritePayload(const bool Payload, TArray<uint8>& Data, int32& Offset)
{
	Data[Offset] = (bool) Payload;
	Offset += 1;
}


void UClientServerMessage::ReadPayload(const TArray<uint8>& Data, bool& Payload, int32& Offset)
{
	Payload = (bool) Data[Offset];
	Offset += 1;
}


int32 UClientServerMessage::CalculatePayloadSize(const uint8 Payload)
{
	return 1;
}

void UClientServerMessage::WritePayload(const uint8 Payload, TArray<uint8>& Data, int32& Offset)
{
	Data[Offset] = Payload;
	Offset += 1;
}


void UClientServerMessage::ReadPayload(const TArray<uint8>& Data, uint8& Payload, int32& Offset)
{
	Payload = Data[Offset];
	Offset += 1;
}





int32 UClientServerMessage::CalculatePayloadSize(const int32 Payload)
{
	return 4;
}

void UClientServerMessage::WritePayload(const int32 Payload, TArray<uint8>& Data, int32& Offset)
{
	for (uint8 i = 0; i < 4; ++i)
	{
		Data[i + Offset] = Payload >> i * 8;
	}
	Offset += 4;
}


void UClientServerMessage::ReadPayload(const TArray<uint8>& Data, int32& Payload, int32& Offset)
{
	Payload = 0;

	for (uint8 i = 0; i < 4; ++i)
	{
		Payload |= Data[i + Offset] << i * 8;
	}
	Offset += 4;
}

