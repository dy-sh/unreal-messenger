// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatGameMode.h"
#include "ChatPlayerController.h"
#include "Messenger/Chat/Widgets/ChatHUD.h"


AChatGameMode::AChatGameMode()
{
	PlayerControllerClass = AChatPlayerController::StaticClass();
	HUDClass = AChatHUD::StaticClass();
	DefaultPawnClass = nullptr;
}



