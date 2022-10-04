// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API UChatGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FChatSettings LoadSettings() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings(const FChatSettings& Settings);
};
