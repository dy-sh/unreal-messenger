// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatServerComponent.generated.h"

class UChatComponent;
class UAuthorizationComponent;
class UChatRoom;
class UChatUser;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UChatServerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool CreateRoom(const FChatRoomSettings& Settings, FString& RoomId);

	UFUNCTION(BlueprintCallable)
	UChatUser* AddUser(UChatComponent* ChatComponent, UAuthorizationComponent* AuthorizationComponent);

	UFUNCTION(BlueprintCallable)
	bool RemoveUser(UChatComponent* ChatComponent, UAuthorizationComponent* AuthorizationComponent);

	UFUNCTION(BlueprintCallable)
	UChatRoom* GetRoom(const FString& RoomId) const;

	UFUNCTION(BlueprintCallable)
	UChatUser* GetUser(const FString& UserId) const;
	
private:
	UPROPERTY()
	TMap<FString, TObjectPtr<UChatRoom>> ChatRooms;

	UPROPERTY()
	TMap<FString, TObjectPtr<UChatUser>> Users;
};
