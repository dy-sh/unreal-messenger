// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ChatTypes.h"
#include "Protocol/UploadFileRequest.h"
#include "Protocol/UploadFileResponse.h"
#include "UObject/Object.h"
#include "FileTransferSubsystem.generated.h"

class UConnectionBase;
class UConnectionHandler;
class UConnectionTcpServer;
class UChatSubsystem;

UCLASS()
class MESSENGER_API UFileTransferSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="FileTransferSubsystem")
	void StartServer(const int32 Port = 3000);

	UFUNCTION(BlueprintCallable, Category="FileTransferSubsystem")
	void StopServer();

	UFUNCTION()
	void OnConnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnDisconnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray);

	FString GetNotExistFileName(const FString& FilePath) const;

	UConnectionHandler* GetConnectionHandler() const { return ConnectionHandler; }

	UFUNCTION(BlueprintPure, Category="FileTransferSubsystem")
	int32 GetServerPort() const { return ServerPort; }


	UFUNCTION(BlueprintPure, Category="FileTransferSubsystem")
	bool IsServerStarted() const { return ConnectionHandler != nullptr; }

	bool SaveReceivedFile(FUploadFileRequestPayload Request, FString& FilePath);
protected:
	int32 ServerPort = 3000;

	UPROPERTY()
	UChatSubsystem* ChatSubsystem;

	UPROPERTY()
	UConnectionHandler* ConnectionHandler;

	UPROPERTY()
	UConnectionTcpServer* Server;

	FTimerHandle StartServerTimerHandle;

	void ReceiveUploadFileRequest(UConnectionBase* Connection, const TArray<uint8>& ByteArray);

	void ReceiveDownloadFileRequest(UConnectionBase* Connection, const TArray<uint8> ByteArray);

	EClientServerMessageType ParseMessageType(const TArray<uint8>& ByteArray) const;
};
