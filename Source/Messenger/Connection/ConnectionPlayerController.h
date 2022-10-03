// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionPlayerController.generated.h"

UCLASS()
class MESSENGER_API AConnectionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AConnectionPlayerController();

protected:
	virtual void BeginPlay() override;
};
