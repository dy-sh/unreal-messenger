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
		if (const auto* World = GetWorld())
		{
			if (const auto* GameMode = Cast<AConnectionGameMode>(World->GetAuthGameMode()))
			{
				ConnectionWidget->Configure(GameMode->SessionParams);
				ConnectionWidget->AddToViewport();
			}
		}
	}
}


