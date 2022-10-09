// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatPlayerController.h"
#include "Messenger/Authorization/AuthorizationComponent.h"
#include "Messenger/Chat/Components/ChatComponent.h"
#include "Messenger/Chat/Components/FileTransferComponent.h"
#include "Messenger/Chat/Components/RoomComponent.h"


AChatPlayerController::AChatPlayerController()
{
	AuthorizationComponent = CreateDefaultSubobject<UAuthorizationComponent>(TEXT("AuthorizationComponent"));
	ChatComponent = CreateDefaultSubobject<UChatComponent>(TEXT("ChatComponent"));
	ChatRoomComponent = CreateDefaultSubobject<URoomComponent>(TEXT("ChatRoomComponent"));
	FileTransferComponent = CreateDefaultSubobject<UFileTransferComponent>(TEXT("FileTransferComponent"));
}
