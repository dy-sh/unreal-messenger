// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatRoomWidget.h"

#include "Messenger/Chat/Components/ChatComponent.h"


void UChatRoomWidget::EnterRoom()
{
	if (!GetWorld()) return;

	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (auto* RoomComponent = PlayerController->FindComponentByClass<URoomComponent>())
		{
			RoomComponent->ServerEnterRoom(RoomId);
		}
	}
}
