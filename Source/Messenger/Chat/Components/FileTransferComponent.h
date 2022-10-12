// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Messenger/Chat/ChatTypes.h"
#include "Messenger/Chat/Protocol/UploadFileRequest.h"
#include "FileTransferComponent.generated.h"

class UConnectionBase;
class UConnectionHandler;
class UConnectionTcpClient;
class UFileTransferServerComponent;
class UOnlineSessionsSubsystem;
class UChatComponent;


UENUM(BlueprintType)
enum class EFileTransferringState:uint8
{
	None,
	SendingFile,
	DownloadingFile,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UFileTransferComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FileTransferComponent")
	int32 ServerPort = 3000;

	UFileTransferComponent();

	UFUNCTION(BlueprintCallable, Category="FileTransferComponent")
	bool SendFileToServer(const FString& RoomId, const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category="FileTransferComponent")
	bool DownloadFileFromServer(const FTransferredFileInfo& FileInfo);

	UFUNCTION(BlueprintPure, Category="FileTransferComponent")
	EFileTransferringState GetState() const { return State; }


protected:
	UPROPERTY()
	UConnectionHandler* ConnectionHandler;
	UPROPERTY()
	UConnectionTcpClient* Client;
	UPROPERTY()
	UFileTransferServerComponent* FileTransferServerComponent;
	UPROPERTY()
	UOnlineSessionsSubsystem* OnlineSessionsSubsystem;
	UPROPERTY()
	UChatComponent* ChatComponent;

	FUploadFileRequestPayload FileToSend;
	FTransferredFileInfo FileToDownload;

	EFileTransferringState State;


	virtual void BeginPlay() override;

	bool ConnectToServer(const FString& IpAddress, const int32 Port);
	void CloseConnection();

	UFUNCTION()
	void OnConnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnDisconnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray);

	EClientServerMessageType ParseMessageType(const TArray<uint8>& ByteArray) const;
	
	void ReceiveDownloadFileResponse(const TArray<uint8> ByteArray);
	FString GetNotExistFileName(const FString& FilePath) const;
};

