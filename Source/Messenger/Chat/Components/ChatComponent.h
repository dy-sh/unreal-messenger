// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Messenger/Authorization/AuthorizationTypes.h"
#include "Messenger/Chat/ChatTypes.h"
#include "Messenger/Chat/User/ChatUser.h"
#include "ChatComponent.generated.h"

class UChatServerComponent;
class UAuthorizationComponent;
class UChatUser;
class URoomComponent;
class UFileTransferComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFileInfoReceived, const FTransferredFileInfo&, FileInfo);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnMessageReceived OnMessageReceived;

	UPROPERTY(BlueprintAssignable)
	FOnFileInfoReceived OnFileInfoReceived;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChatUser* ChatUser;


	UChatComponent();

	virtual void BeginPlay() override;


	UFUNCTION()
	void OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys& ClientKeys,
		const FEncryptionKeys& ServerKeys);

	UFUNCTION(BlueprintCallable)
	void DisconnectFromServer();


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerGetUserInfo();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientSetUserInfo(const FUserInfo& Info, const FUserPrivateInfo& PrivateInfo);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSendMessage(const FString& Text);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientReceiveMessage(const FChatMessage& Message);


	UFUNCTION(BlueprintCallable)
	void SendEncryptedMessageToServer(const FString& Text);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSendEncryptedMessage(const FString& EncryptedText, const int32 PayloadSize);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientReceiveEncryptedMessage(const FChatMessage& EncryptedMessage, const int32 PayloadSize);

	
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientReceiveFileInfo(const FTransferredFileInfo& FileInfo);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerGetFile(const FTransferredFileInfo& FileInfo);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerChangeUserName(const FString& NewName);

	UFUNCTION(BlueprintPure)
	const FUserInfo& GetUserInfo() const { return ChatUser ? ChatUser->Info : UserInfo; }


	UFUNCTION(BlueprintPure)
	const FUserPrivateInfo& GetUserPrivateInfo() const { return ChatUser ? ChatUser->PrivateInfo : UserPrivateInfo; }


	UFUNCTION(BlueprintPure)
	FORCEINLINE URoomComponent* GetRoomComponent() const { return RoomComponent; }


	UFUNCTION(BlueprintPure)
	FORCEINLINE UAuthorizationComponent* GetAuthorizationComponent() const { return AuthorizationComponent; }

private:
	UPROPERTY()
	UAuthorizationComponent* AuthorizationComponent;
	UPROPERTY()
	UChatServerComponent* ChatServerComponent;
	UPROPERTY()
	URoomComponent* RoomComponent;
	UPROPERTY()
	UFileTransferComponent* FileTransferComponent;

	FUserInfo UserInfo;
	FUserPrivateInfo UserPrivateInfo;

	UFUNCTION()
	void SendMessageToAllUsersInRoom(const FString& Text, const bool SendEncrypted);
};
