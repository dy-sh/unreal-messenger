// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ClientServerMessage.h"
#include "NetworkUtils.h"


template <class T>
void UClientServerMessage<T>::WritePayloadToPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Payload,
	TArray<uint8>& Package, int32& Offset)
{
	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Package[i + Offset] = Payload.Num() >> i * 8;
	}
	Offset += DataSizeBitDepth;

	FMemory::Memcpy(Package.GetData() + Offset, Payload.GetData(), Payload.Num());
	Offset += Payload.Num();
}


template <class T>
void UClientServerMessage<T>::ReadPayloadFromPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Package, TArray<uint8>& Payload,
	int32& Offset)
{
	int32 Size = 0;

	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Size |= Package[i + Offset] << i * 8;
	}
	Offset += DataSizeBitDepth;

	Payload.SetNum(Size);
	FMemory::Memcpy(Payload.GetData(), Package.GetData() + Offset, Size);
	Offset += Size;
}
