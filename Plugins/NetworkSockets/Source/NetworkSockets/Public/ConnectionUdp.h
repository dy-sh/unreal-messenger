// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionBase.h"
#include "ConnectionUdp.generated.h"


UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionUdp : public UConnectionBase
{
	GENERATED_BODY()

public:
	void Initialize(const FIPv4Endpoint Endpoint);

	virtual bool GetIpAddressAsByteArray(TArray<uint8>& IpAddress) const override;
	virtual FString GetIpAddress() const override;

	void ReceiveData(const uint8* ByteArray, const int32 Size);

protected:
	FIPv4Endpoint ConnectionEndpoint;
};
