// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionBase.h"
#include "ConnectionUdpClient.generated.h"


UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionUdpClient : public UConnectionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets|UdpClient")
	FString ServerIpAddress = "localhost";

	UFUNCTION(BlueprintPure, Category = "NetworkSockets|UdpClient")
	static UConnectionUdpClient* CreateUdpClient(const FString& IpAddress = "localhost", const int32 Port = 3000);

	UFUNCTION(BlueprintCallable, Category = "NetworkSockets|UdpClient")
	virtual void Initialize(const FString& IpAddress = "localhost", const int32 Port = 3000);

	virtual void Open() override;
	virtual void Close() override;
	virtual int32 Send(const TArray<uint8>& ByteArray) override;
	virtual int32 SendTo(const TArray<uint8>& ByteArray, const FIPv4Endpoint& Target) override;

protected:
	FIPv4Endpoint ServerEndpoint;
	virtual void OnSocketDisconnected(UConnectionBase* Connection) override;
};
