// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Messenger/Chat/ChatTypes.h"
#include "AddRoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API UAddRoomWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
	UFUNCTION(BlueprintCallable, Category="Chat")
	void CreateRoom(const FChatRoomSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="Chat")
	void JoinRoom(const FString& RoomId);
	
private:
	UPROPERTY()
	class URoomComponent* RoomComponent;
};
