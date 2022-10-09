// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionBase.h"
#include "Worker.h"
#include "ConnectionTcpServer.generated.h"

class UConnectionTcp;

UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionTcpServer : public UConnectionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NetworkSockets|TcpServer")
	int32 MaxBacklog = 10;
	
	UFUNCTION(BlueprintPure, Category = "NetworkSockets|TcpServer")
	static UConnectionTcpServer* CreateTcpServer(int32 Port = 3000);
 
	UFUNCTION(BlueprintCallable, Category = "NetworkSockets|TcpServer")
	void Initialize(int32 Port = 3000);

	virtual void Open() override;
	virtual void Close() override;
	virtual int32 Send(const TArray<uint8>& ByteArray) override;

protected:
	TArray<UConnectionTcp*> OpenedConnections;
	
	EWorkerResult ListenPort();
	virtual void OnSocketDisconnected(UConnectionBase* Connection) override;

};
