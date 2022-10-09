// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionHandler.generated.h"

class UConnectionBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectionHandlerConnected, UConnectionBase*, Connection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectionHandlerDisconnected, UConnectionBase*, Connection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnConnectionHandlerReceivedData, UConnectionBase*, Connection, const TArray<uint8>&, ByteArray);

UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionHandler : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "NetworkSockets")
	bool bUseGameThread{true};

	UPROPERTY(BlueprintReadOnly, Category = "NetworkSockets")
	TArray<UConnectionBase*> Connections;

	UPROPERTY(BlueprintAssignable, Category = "NetworkSockets")
	FOnConnectionHandlerConnected OnConnected;

	UPROPERTY(BlueprintAssignable, Category = "NetworkSockets")
	FOnConnectionHandlerDisconnected OnDisconnected;

	UPROPERTY(BlueprintAssignable, Category = "NetworkSockets")
	FOnConnectionHandlerReceivedData OnReceivedData;

	UFUNCTION(BlueprintPure, Category = "NetworkSockets")
	static UConnectionHandler* CreateConnectionHandler();
	
	UFUNCTION(BlueprintCallable, Category = "NetworkSockets")
	UConnectionHandler* Open(UConnectionBase* Connection);

	UFUNCTION(BlueprintCallable, Category = "NetworkSockets")
	void Close();

	UFUNCTION(BlueprintCallable, Category = "NetworkSockets")
	void Send(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintCallable, Category = "NetworkSockets")
	void SendTo(const TArray<uint8>& ByteArray, UConnectionBase* Target);

	UFUNCTION(BlueprintPure, Category = "NetworkSockets")
	bool IsConnected();

	virtual void BeginDestroy() override;


private:
	UPROPERTY()
	UConnectionBase* HandledConnection;
	
	bool bIsDestroying;
	
	void ExecuteDelegate(TFunction<void()> Delegate) const;

	UFUNCTION()
	void OnSocketConnected(UConnectionBase* ConnectionBase);
	UFUNCTION()
	void OnSocketDisconnected(UConnectionBase* ConnectionBase);
	UFUNCTION()
	void OnSocketReceivedData(UConnectionBase* ConnectionBase, const TArray<uint8>& Array);
};

