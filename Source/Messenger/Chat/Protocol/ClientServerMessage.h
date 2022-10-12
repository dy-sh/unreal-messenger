// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.generated.h"

constexpr uint8 BYTE_ARRAY_SIZE_BIT_DEPTH = 4;

UCLASS()
class UClientServerMessage : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	const TArray<uint8>& GetByteArray() const { return DataByteArray; }


	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	virtual uint8 GetMessageType() const { return 0; }


	static int32 CalculatePayloadSize(const TArray<uint8>& Payload);
	static void WritePayload(const TArray<uint8>& Payload, TArray<uint8>& Data, int32& Offset);
	static void ReadPayload(const TArray<uint8>& Data, TArray<uint8>& Payload, int32& Offset);

	static int32 CalculatePayloadSize(const FString& Payload);
	static void WritePayload(const FString& Payload, TArray<uint8>& Data, int32& Offset);
	static void ReadPayload(const TArray<uint8>& Data, FString& Payload, int32& Offset);
	
	static int32 CalculatePayloadSize(const bool Payload);
	static void WritePayload(const bool Payload, TArray<uint8>& Data, int32& Offset);
	static void ReadPayload(const TArray<uint8>& Data, bool& Payload, int32& Offset);
	
	static int32 CalculatePayloadSize(const uint8 Payload);
	static void WritePayload(const uint8 Payload, TArray<uint8>& Data, int32& Offset);
	static void ReadPayload(const TArray<uint8>& Data, uint8& Payload, int32& Offset);

	static int32 CalculatePayloadSize(const int32 Payload);
	static void WritePayload(const int32 Payload, TArray<uint8>& Data, int32& Offset);
	static void ReadPayload(const TArray<uint8>& Data, int32& Payload, int32& Offset);
	

protected:
	TArray<uint8> DataByteArray;
};
