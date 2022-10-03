// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ConnectionGameMode.h"

#include "ConnectionHUD.h"
#include "ConnectionPlayerController.h"



AConnectionGameMode::AConnectionGameMode()
{
	HUDClass = AConnectionHUD::StaticClass();
	PlayerControllerClass = AConnectionPlayerController::StaticClass();
}

void AConnectionGameMode::BeginPlay()
{
	Super::BeginPlay();
}
