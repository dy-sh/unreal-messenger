// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FileTransferComponent.generated.h"

class UConnectionBase;
class UConnectionHandler;
class UConnectionTcpClient;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UFileTransferComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FileTransferComponent")
	int32 ServerPort;

	UFUNCTION(BlueprintCallable, Category="FileTransferComponent")
	bool ConnectToServer(const FString& IpAddress, const int32 Port);

	UFUNCTION(BlueprintCallable, Category="FileTransferComponent")
	void CloseConnection();

	UFUNCTION()
	void OnConnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnDisconnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray);

	UConnectionHandler* GetConnectionHandler() const { return ConnectionHandler; }
protected:
	UConnectionHandler* ConnectionHandler;
	UConnectionTcpClient* Client;

	virtual void BeginPlay() override;
};
