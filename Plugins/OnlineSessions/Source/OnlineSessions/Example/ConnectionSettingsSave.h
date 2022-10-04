// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ConnectionSettingsSave.generated.h"

/**
 * 
 */
UCLASS()
class ONLINESESSIONS_API UConnectionSettingsSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Connection Settings")
	FString ServerIP = "127.0.0.1";
};
