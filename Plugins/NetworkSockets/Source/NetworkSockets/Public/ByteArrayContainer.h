// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"

class FByteArrayContainer
{
public:
	uint8* GetByteArrayPtr() { return ByteArrayData.GetData(); }
	TArray<uint8> GetPartOfByteArray(const int32 Offset, const int32 Length);

	uint8* AddByteArraySize(const int32 Size);
	void SetByteArraySize(const int32 Size) { ByteArrayData.SetNum(Size); }
	int32 GetByteArraySize() const { return ByteArrayData.Num(); }

	void Add(const uint8* SrcByteArray, const int32 Size);
	void CopyFrom(const uint8* SrcByteArray, const int32 Size, const int32 DstOffset = 0);
	void RemoveFromStart(const int32 Length);
	void Clear();

	uint8 operator [](const int32 Index) { return ByteArrayData[Index]; }

private:
	TArray<uint8> ByteArrayData;
};
