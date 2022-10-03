// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatSettingsSave.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API UChatSettingsSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category="Settings")
	FChatSettings Settings;
};
