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
class UFileTransferSubsystem;
class UOnlineSessionsSubsystem;
class UChatComponent;




DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUploadingFileStarted, const FTransferredFileInfo&, FileInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUploadingFileComplete, const FTransferredFileInfo&, FileInfo, bool, bSucces);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDownloadingFileStarted, const FTransferredFileInfo&, FileInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDownloadingFileComplete, const FTransferredFileInfo&, FileInfo, bool, bSucces);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UFileTransferComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FileTransferComponent")
	int32 ServerPort = 3000;

	UFileTransferComponent();

	FOnUploadingFileStarted OnUploadingFileStarted;
	FOnUploadingFileComplete OnUploadingFileComplete;
	FOnDownloadingFileStarted OnDownloadingFileStarted;
	FOnDownloadingFileComplete OnDownloadingFileComplete;

	UFUNCTION(Server, Reliable)
	void ServerRequestUploadingFile();

	UFUNCTION(Client, Reliable)
	void ClientResponseUploadingFile(const FString& FileId);

	UFUNCTION(BlueprintCallable, Category="FileTransferComponent")
	bool SendFileToServer(const FString& RoomId, const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category="FileTransferComponent")
	bool DownloadFileFromServer(const FTransferredFileInfo& FileInfo);

	UFUNCTION(BlueprintPure, Category="FileTransferComponent")
	ETransferringFileState GetState() const { return ProceedingFileInfo.State; }


protected:
	UPROPERTY()
	UConnectionHandler* ConnectionHandler;
	UPROPERTY()
	UConnectionTcpClient* Client;
	UPROPERTY()
	UFileTransferSubsystem* FileTransferSubsystem;
	UPROPERTY()
	UOnlineSessionsSubsystem* OnlineSessionsSubsystem;
	UPROPERTY()
	UChatComponent* ChatComponent;

	FTransferredFileInfo ProceedingFileInfo;
	TArray<uint8> ProceedingFileContent;


	virtual void BeginPlay() override;

	void ConnectToServer(const FString& IpAddress, const int32 Port);
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
