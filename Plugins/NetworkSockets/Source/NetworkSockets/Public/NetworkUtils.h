// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "NetworkUtils.generated.h"


UCLASS(BlueprintType)
class NETWORKSOCKETS_API UNetworkUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "NetworkSockets|Utils")
	static void StringToByteArray(const FString& String, TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "NetworkSockets|Utils")
	static FString ByteArrayToString(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "NetworkSockets|Utils")
	static bool IpStringToByteArray(FString IpAddress, TArray<uint8>& ByteArray);

	static bool GetEndpoint(const FString& IpAddress, const int32 Port, FIPv4Endpoint& OutEndpoint);
};
