// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineSessions/Types.h"
#include "ConnectionGameMode.generated.h"

UCLASS()
class MESSENGER_API AConnectionGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AConnectionGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=OnlineSessions)
	FSessionParams SessionParams;

protected:
	virtual void BeginPlay() override;
};
