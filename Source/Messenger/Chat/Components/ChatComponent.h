// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "RoomComponent.h"
#include "Messenger/Authorization/AuthorizationTypes.h"
#include "Messenger/Chat/ChatTypes.h"
#include "Messenger/Chat/User/ChatUser.h"
#include "ChatComponent.generated.h"

class UChatServerComponent;
class UAuthorizationComponent;
class UChatUser;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FChatMessage&, Message);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnMessageReceived OnMessageReceived;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChatUser* ChatUser;

	UChatComponent();

	virtual void BeginPlay() override;


	UFUNCTION()
	void OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys& ClientKeys,
		const FEncryptionKeys& ServerKeys);


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


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerChangeUserName(const FString& NewName);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE URoomComponent* GetRoomComponent() const { return RoomComponent; }


	UFUNCTION(BlueprintCallable)
	const FUserInfo& GetUserInfo() const { return ChatUser ? ChatUser->Info : UserInfo; }


	UFUNCTION(BlueprintCallable)
	const FUserPrivateInfo& GetUserPrivateInfo() const { return ChatUser ? ChatUser->PrivateInfo : UserPrivateInfo; }


private:
	UAuthorizationComponent* AuthorizationComponent;
	UChatServerComponent* ChatServerComponent;
	URoomComponent* RoomComponent;
	FUserInfo UserInfo;
	FUserPrivateInfo UserPrivateInfo;

	UFUNCTION()
	void SendMessageToAllUsersInRoom(const FString& Text, const bool SendEncrypted);
};
