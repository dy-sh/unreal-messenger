// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Messenger/Chat/ChatTypes.h"
#include "RoomOptions.generated.h"


UCLASS()
class MESSENGER_API URoomOptions : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintPure, Category="Chat")
	void GetRoomId(FString& RoomId);

	UFUNCTION(BlueprintPure, Category="Chat")
	void GetRoomSettings(FChatRoomSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="Chat")
	void EditRoomSettings(const FChatRoomSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="Chat")
	void LeaveRoom();

private:
	UPROPERTY()
	class URoomComponent* RoomComponent;
};
