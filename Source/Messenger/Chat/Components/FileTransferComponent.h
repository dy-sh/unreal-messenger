// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "FileDataPackage.h"
#include "Components/ActorComponent.h"
#include "OnlineSessions/OnlineSessionsSubsystem.h"
#include "FileTransferComponent.generated.h"

class UConnectionBase;
class UConnectionHandler;
class UConnectionTcpClient;
class UFileTransferServerComponent;




UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UFileTransferComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FileTransferComponent")
	int32 ServerPort = 3000;

	UFileTransferComponent();

	UFUNCTION(BlueprintCallable, Category="FileTransferComponent")
	bool SendFile(const FString& FilePath);

	UFUNCTION(Server, Reliable)
	void ServerRequestFileTransferring(const FString& FileName, const int32 FileSize, const FString& RoomId);

	UFUNCTION(Client, Reliable)
	void ClientResponseFileTransferring(const FString& FileTransferId);

	UFUNCTION(BlueprintPure)
	bool IsSendingFile() const;

protected:
	UConnectionHandler* ConnectionHandler;
	UConnectionTcpClient* Client;
	UFileTransferServerComponent* FileTransferServerComponent;
	UOnlineSessionsSubsystem* OnlineSessionsSubsystem;

	FFileDataPackageInfo FileToSend;


	virtual void BeginPlay() override;

	bool ConnectToServer(const FString& IpAddress, const int32 Port);
	void CloseConnection();

	UFUNCTION()
	void OnConnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnDisconnected(UConnectionBase* Connection);
	UFUNCTION()
	void OnReceivedData(UConnectionBase* Connection, const TArray<uint8>& ByteArray);


};
