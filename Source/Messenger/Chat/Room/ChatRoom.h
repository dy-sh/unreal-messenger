// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatRoom.generated.h"

class UChatComponent;

UCLASS()
class MESSENGER_API UChatRoom : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FString GetRoomId() const { return RoomId; }


	UFUNCTION(BlueprintCallable)
	void SetRoomId(const FString& Id) { RoomId = Id; }


	UFUNCTION(BlueprintCallable)
	FChatRoomSettings GetRoomSettings() const { return RoomSettings; }


	UFUNCTION(BlueprintCallable)
	void SetRoomSettings(const FChatRoomSettings& Settings) { RoomSettings = Settings; }


	UFUNCTION(BlueprintCallable)
	void AddMessage(const FChatMessage& Message) { ChatMessages.Add(Message); }


	UFUNCTION(BlueprintCallable)
	const TArray<FChatMessage> GetLastMessages(const int32& Number);


	UFUNCTION(BlueprintCallable)
	void JoinUser(const FString& UserId);


	UFUNCTION(BlueprintCallable)
	void LeaveUser(const FString& UserId);

	UFUNCTION(BlueprintCallable)
	const TArray<FString>& GetJoinedUserIds() { return JoinedUserIds; }


	UFUNCTION(BlueprintCallable)
	void EnterUser(UChatComponent* ChatComponent);


	UFUNCTION(BlueprintCallable)
	void ExitUser(UChatComponent* ChatComponent);

	UFUNCTION(BlueprintCallable)
	const TArray<UChatComponent*>& GetActiveChatComponents() { return ActiveChatComponents; }


	UFUNCTION(BlueprintCallable)
	TArray<UChatComponent*> GetActiveChatComponentsOfUser(FString UserId);


private:
	FString RoomId;
	FChatRoomSettings RoomSettings;
	TArray<FChatMessage> ChatMessages;
	TArray<FString> JoinedUserIds;
	TArray<UChatComponent*> ActiveChatComponents;
};
