// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ByteArrayContainer.h"


TArray<uint8> FByteArrayContainer::GetPartOfByteArray(const int32 Offset, const int32 Length)
{
	// prevent overflow
	const int32 ArrSize = Offset + Length > GetByteArraySize() ? GetByteArraySize() - Offset : Length;

	TArray<uint8> Arr = TArray<uint8>();
	Arr.SetNum(ArrSize);
	FMemory::Memcpy(Arr.GetData(), ByteArrayData.GetData() + Offset, ArrSize);
	return Arr;
}


uint8* FByteArrayContainer::AddByteArraySize(const int32 Size)
{
	ByteArrayData.SetNum(ByteArrayData.Num() + Size);
	return ByteArrayData.GetData() + (ByteArrayData.Num() - Size);
}


void FByteArrayContainer::Add(const uint8* SrcByteArray, const int32 Size)
{
	const int32 Offset = GetByteArraySize();
	CopyFrom(SrcByteArray, Size, Offset);
}


void FByteArrayContainer::CopyFrom(const uint8* SrcByteArray, const int32 Size, const int32 DstOffset)
{
	const int32 NewSize = FMath::Max(GetByteArraySize(), Size + DstOffset); // increase size if overflow
	SetByteArraySize(NewSize);
	FMemory::Memcpy(ByteArrayData.GetData() + DstOffset, SrcByteArray, Size);
}


void FByteArrayContainer::RemoveFromStart(int32 Length)
{
	const int32 NewSize = GetByteArraySize() - Length;
	TArray<uint8> Arr = TArray<uint8>();
	Arr.SetNum(NewSize);
	FMemory::Memcpy(Arr.GetData(), ByteArrayData.GetData() + Length, NewSize);
	ByteArrayData = Arr;
}


void FByteArrayContainer::Clear()
{
	ByteArrayData.Empty();
}
