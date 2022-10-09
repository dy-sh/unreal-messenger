// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "DataPackage.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "ConnectionBase.generated.h"

class UConnectionBase;
class FWorker;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSocketConnected, UConnectionBase*,Connection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSocketDisconnected, UConnectionBase*,Connection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSocketReceivedData, UConnectionBase*,Connection, const TArray<uint8>&,ByteArray);

UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets|Socket")
	int32 ConnectionPort = 3000;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets|Socket")
	int32 ReceiveBufferSize = 10000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets|Socket")
	int32 SendBufferSize = 10000;

	FDataPackage DataPackage;
	
	FOnSocketConnected OnConnected;
	FOnSocketDisconnected OnDisconnected;
	FOnSocketReceivedData OnReceivedData;

	virtual void BeginDestroy() override;

	virtual void Open(){}
	virtual void Close(){}
	virtual int32 Send(const TArray<uint8>& ByteArray);
	virtual int32 SendTo(const TArray<uint8>& ByteArray, const FIPv4Endpoint& Target);

	UFUNCTION(BlueprintPure, Category = "NetworkSockets|Socket")
	virtual bool GetIpAddressAsByteArray(TArray<uint8>& IpAddress) const;

	UFUNCTION(BlueprintPure, Category = "NetworkSockets|Socket")
	virtual FString GetIpAddress() const;
	
protected:
	FSocket* Socket;
	
	FWorker* Worker;
	FRunnableThread* ConnectionThread;

	virtual void CloseSocket();

	UFUNCTION()
	virtual void OnSocketConnected(UConnectionBase* Connection);
	UFUNCTION()
	virtual void OnSocketDisconnected(UConnectionBase* Connection);
	UFUNCTION()
	virtual void OnSocketReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray);
};
