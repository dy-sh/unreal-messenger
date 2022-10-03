// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ConnectionHUD.generated.h"

UCLASS()
class MESSENGER_API AConnectionHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=OnlineSessions)
	TSubclassOf<class UConnectionWidget> ConnectionWidgetClass;

protected:
	UPROPERTY(BlueprintReadOnly)
	UConnectionWidget* ConnectionWidget;

	virtual void BeginPlay() override;

	void CreateConnectionUI();
};
