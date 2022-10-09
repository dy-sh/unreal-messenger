// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionBase.h"
#include "Worker.h"
#include "ConnectionUdpServer.generated.h"

class UConnectionUdp;

UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionUdpServer : public UConnectionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "NetworkSockets|UdpServer")
	static UConnectionUdpServer* CreateUdpServer(int32 Port = 3000);

	UFUNCTION(BlueprintCallable, Category = "NetworkSockets|UdpServer")
	void Initialize(int32 Port = 3000);

	virtual void Open() override;
	virtual void Close() override;
	virtual int32 Send(const TArray<uint8>& ByteArray) override;
	virtual int32 SendTo(const TArray<uint8>& ByteArray, const FIPv4Endpoint& Target) override;

protected:
	TMap<FIPv4Endpoint, UConnectionUdp*> OpenedConnections;
	FCriticalSection Lock;
	bool bIsClosingSocket = false;
	ISocketSubsystem* SocketSubsystem;
	
	EWorkerResult ReceiveData();
	virtual void OnSocketDisconnected(UConnectionBase* Connection) override;
};
