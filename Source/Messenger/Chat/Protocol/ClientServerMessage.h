// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.generated.h"

constexpr int32 DATA_SIZE_BIT_DEPTH = 4;

UCLASS()
class UClientServerMessage : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	const TArray<uint8>& GetByteArray() const { return DataByteArray; }


	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	int32 GetMessageType() const { return MessType; }


	static void WritePayloadToDataByteArray(const int32 DataSizeBitDepth, const TArray<uint8>& Payload, TArray<uint8>& Data,
		int32& Offset);
	static void ReadPayloadFromDataByteArray(const int32 DataSizeBitDepth, const TArray<uint8>& Data, TArray<uint8>& Payload,
		int32& Offset);
	static void WritePayloadToDataByteArray(const int32 DataSizeBitDepth, const int32 Payload, TArray<uint8>& Data,
		int32& Offset);
	static void ReadPayloadFromDataByteArray(const int32 DataSizeBitDepth, const TArray<uint8>& Data, int32& Payload,
		int32& Offset);
	static void WritePayloadToDataByteArray(const int32 DataSizeBitDepth, const bool Payload,
		TArray<uint8>& Data, int32& Offset);
	static void ReadPayloadFromDataByteArray(const int32 DataSizeBitDepth, const TArray<uint8>& Data,
		bool& Payload, int32& Offset);
protected:
	int32 MessType{0};
	TArray<uint8> DataByteArray;
};
