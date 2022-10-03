// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ConnectionHUD.h"

#include "ConnectionGameMode.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessions/Example/ConnectionWidget.h"


void AConnectionHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateConnectionUI();
}


void AConnectionHUD::CreateConnectionUI()
{
	ConnectionWidget = CreateWidget<UConnectionWidget>(GetOwningPlayerController(), ConnectionWidgetClass);
	if (ConnectionWidget)
	{
		if (auto* World = GetWorld())
		{
			if (auto* GameMode = Cast<AConnectionGameMode>(World->GetAuthGameMode()))
			{
				ConnectionWidget->Configure(GameMode->SessionParams);
				ConnectionWidget->AddToViewport();
			}
		}
	}
}


