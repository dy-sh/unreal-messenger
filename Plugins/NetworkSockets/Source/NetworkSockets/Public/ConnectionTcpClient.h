// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionTcp.h"
#include "ConnectionTcpClient.generated.h"

UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionTcpClient : public UConnectionTcp
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets|TcpClient")
	FString ServerIpAddress = "localhost";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets|TcpClient")
	bool bRetryToConnect = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets|TcpClient")
	bool bAutoReconnectIfDisconnected = false;

	UFUNCTION(BlueprintPure, Category = "NetworkSockets|TcpClient")
	static UConnectionTcpClient* CreateTcpClient(const FString& IpAddress = "localhost", const int32 Port = 3000,
	                                             const bool bRetry = false, const bool bAutoReconnect = false);

	UFUNCTION(BlueprintCallable, Category = "NetworkSockets|TcpClient")
	void Initialize(const FString& IpAddress = "localhost", const int32 Port = 3000, const bool bRetry = false,
	                const bool bAutoReconnect = false);

	virtual void Open() override;
	virtual void Close() override;

protected:
	FIPv4Endpoint ServerEndpoint;

	EWorkerResult TryToConnect();
	virtual void OnSocketDisconnected(UConnectionBase* Connection) override;
};
