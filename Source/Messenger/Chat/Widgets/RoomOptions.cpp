// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "RoomOptions.h"

#include "Messenger/Chat/Components/RoomComponent.h"


bool URoomOptions::Initialize()
{
	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			if (auto* Comp = PlayerController->FindComponentByClass<URoomComponent>())
			{
				RoomComponent = Comp;
			}
		}
	}

	return Super::Initialize();
}




void URoomOptions::GetRoomId(FString& RoomId)
{
	if (RoomComponent)
	{
		RoomId = RoomComponent->GetActiveRoomId();
	}
}


void URoomOptions::GetRoomSettings(FChatRoomSettings& Settings)
{
	if (RoomComponent)
	{
		Settings = RoomComponent->GetActiveRoomSettings();
	}
}


void URoomOptions::EditRoomSettings(const FChatRoomSettings& Settings)
{
	if (RoomComponent)
	{
		RoomComponent->ServerEditRoomSettings(RoomComponent->GetActiveRoomId(), Settings);
	}
}


void URoomOptions::LeaveRoom()
{
	if (RoomComponent)
	{
		RoomComponent->ServerLeaveRoom(RoomComponent->GetActiveRoomId());
	}
}
