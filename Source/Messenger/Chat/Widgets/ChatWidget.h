// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Messenger/Authorization/AuthorizationTypes.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatWidget.generated.h"


/**
 * 
 */
UCLASS()
class MESSENGER_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;

	UFUNCTION()
	void OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys& ClientEncryptionKeys,
		const FEncryptionKeys& ServerEncryptionKeys);

	UFUNCTION()
	void OnMessageReceived(const FChatMessage& Message);

	UFUNCTION()
	void OnJoinRoom(const FString& RoomId, const FUserInfo& User);

	UFUNCTION()
	void OnLeaveRoom(const FString& RoomId, const FUserInfo& User);

	UFUNCTION(BlueprintCallable, Category="Chat")
	void SendEncryptedMessage(const FString& Text);

	UFUNCTION(BlueprintCallable, Category="Chat")
	bool SendFile(const FString& FileName);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Chat")
	void ShowNewMessage(const FChatMessage& Message);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Chat")
	void OnJoinedRoomsUpdated(const FString& ActiveRoomId, const TArray<FString>& Ids, const TArray<FChatRoomSettings>& Settings);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Chat")
	void OnEnterRoom(const FString& RoomId, const TArray<FChatMessage>& Messages);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Chat")
	void OnExitRoom(const FString& RoomId);

	UPROPERTY()
	class UChatComponent* ChatComponent;
	
	UPROPERTY()
	class URoomComponent* RoomComponent;
};



