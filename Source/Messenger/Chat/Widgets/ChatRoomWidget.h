// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatRoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API UChatRoomWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void EnterRoom();

	UFUNCTION(BlueprintCallable)
	FString GetRoomId() const { return RoomId; }

	UFUNCTION(BlueprintCallable)
	void SetRoomId(const FString& Id) { RoomId = Id; }
private:
	FString RoomId;
};
