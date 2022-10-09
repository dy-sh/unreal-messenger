// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ByteArrayContainer.h"

#define HEADER_SIZE 4

class FDataPackage : public FByteArrayContainer
{
public:
	bool GetPayloadSize(int32& Size);
	bool HasReceivedPayload();
	bool DequeueReceivedPayload(TArray<uint8>& ByteArray);
	
	static int32 GetHeaderSize() { return HEADER_SIZE; }
	static TArray<uint8> PayloadToDataPackage(const TArray<uint8>& ByteArray);
};
