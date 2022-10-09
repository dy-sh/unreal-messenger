// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FileTransferServerComponent.generated.h"

class UConnectionBase;
class UConnectionHandler;
class UConnectionTcpServer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UFileTransferServerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FileTransferServerComponent")
	int32 ServerPort = 3000;

	UFUNCTION(BlueprintCallable, Category="FileTransferServerComponent")
	bool StartServer();

	UFUNCTION(BlueprintCallable, Category="FileTransferServerComponent")
	void StopServer();

	UFUNCTION()
	void OnConnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnDisconnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray);

	UConnectionHandler* GetConnectionHandler() const { return ConnectionHandler; }
protected:
	UConnectionHandler* ConnectionHandler;
	UConnectionTcpServer* Server;

	virtual void BeginPlay() override;
};
