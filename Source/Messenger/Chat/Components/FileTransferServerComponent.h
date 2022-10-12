// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Messenger/Chat/ChatTypes.h"
#include "FileTransferServerComponent.generated.h"

class UConnectionBase;
class UConnectionHandler;
class UConnectionTcpServer;
class UChatServerComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UFileTransferServerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FileTransferServerComponent")
	int32 ServerPort = 3000;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="FileTransferServerComponent")
	void StartServer();

	UFUNCTION(BlueprintCallable, Category="FileTransferServerComponent")
	void StopServer();

	UFUNCTION()
	void OnConnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnDisconnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray);

	void SendFileToClient(const FTransferredFileInfo& FileInfo);
	
	void SendFileInfoToAllUsersInRoom(FTransferredFileInfo FileInfo, bool SendEncrypted);
	
	FString GetNotExistFileName(const FString& FilePath) const;
	
	UConnectionHandler* GetConnectionHandler() const { return ConnectionHandler; }
	
protected:
	UPROPERTY()
	UChatServerComponent* ChatServerComponent;

	UPROPERTY()
	UConnectionHandler* ConnectionHandler;

	UPROPERTY()
	UConnectionTcpServer* Server;

	FTimerHandle StartServerTimerHandle;

	void ReceiveUploadedFile(const TArray<uint8>& ByteArray);
};
