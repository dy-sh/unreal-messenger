// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Messenger/Authorization/AuthorizationTypes.h"
#include "Messenger/Chat/ChatTypes.h"
#include "RoomComponent.generated.h"

class UChatServerComponent;
class UAuthorizationComponent;
class UChatComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoinRoom, const FString&, RoomId, const FUserInfo&, User);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLeaveRoom, const FString&, RoomId, const FUserInfo&, User);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnterRoom, const FString&, RoomId, const TArray<FChatMessage>&, Messages);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExitRoom, const FString&, RoomId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnJoinedRoomsUpdated, const FString&, ActiveRoomId, const TArray<FString>&, RoomIds,
	const TArray<FChatRoomSettings>&, RoomSettings);

UCLASS()
class MESSENGER_API URoomComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnJoinRoom OnJoinRoom;

	UPROPERTY(BlueprintAssignable)
	FOnLeaveRoom OnLeaveRoom;

	UPROPERTY(BlueprintAssignable)
	FOnEnterRoom OnEnterRoom;

	UPROPERTY(BlueprintAssignable)
	FOnExitRoom OnExitRoom;

	UPROPERTY(BlueprintAssignable)
	FOnJoinedRoomsUpdated OnJoinedRoomsUpdated;


	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys& ClientEncryptionKeys,
		const FEncryptionKeys& ServerEncryptionKeys);


	UFUNCTION(Server, Reliable)
	void ServerGetJoinedChatRooms();

	UFUNCTION(Client, Reliable)
	void ClientSetJoinedChatRooms(const TArray<FString>& RoomIds, const TArray<FChatRoomSettings>& RoomSettings);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerCreateRoom(const FChatRoomSettings& Settings);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerJoinRoom(const FString& RoomId);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientJoinRoom(const FString& RoomId, const FUserInfo& UserInfo);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerLeaveRoom(const FString& RoomId);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientLeaveRoom(const FString& RoomId, const FUserInfo& UserInfo);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerEnterRoom(const FString& RoomId);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientEnterRoom(const FString& RoomId, const TArray<FChatMessage>& Messages);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerExitRoom(const FString& RoomId);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientExitRoom(const FString& RoomId);


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerEditRoomSettings(const FString& RoomId, const FChatRoomSettings& Settings);


	UFUNCTION(BlueprintCallable)
	const FString& GetActiveRoomId() const { return ActiveRoomId; }


	UFUNCTION(BlueprintCallable)
	const FChatRoomSettings& GetActiveRoomSettings() const { return ActiveRoomSettings; }


private:
	UChatComponent* ChatComponent;
	UAuthorizationComponent* AuthorizationComponent;
	UChatServerComponent* ChatServerComponent;
	FString ActiveRoomId;
	FChatRoomSettings ActiveRoomSettings;
};
