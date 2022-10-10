// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "AddRoomWidget.h"

#include "Messenger/Chat/Components/ChatComponent.h"
#include "Messenger/Chat/Components/RoomComponent.h"


bool UAddRoomWidget::Initialize()
{
	if (const UWorld* World = GetWorld())
	{
		if (const APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			if (auto* Comp = PlayerController->FindComponentByClass<URoomComponent>())
			{
				RoomComponent = Comp;
			}
		}
	}

	return Super::Initialize();
}

void UAddRoomWidget::CreateRoom(const FChatRoomSettings& Settings)
{
	if (RoomComponent)
	{
		RoomComponent->ServerCreateRoom(Settings);
	}
}


void UAddRoomWidget::JoinRoom(const FString& RoomId)
{
	if (RoomComponent)
	{
		RoomComponent->ServerJoinRoom(RoomId);
	}
}
