// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatRoom.h"

#include "Messenger/Chat/Components/ChatComponent.h"


const TArray<FChatMessage> UChatRoom::GetLastMessages(const int32& Number)
{
	TArray<FChatMessage> LastMessages;

	const int32 FirstMessageIndex = FMath::Max(ChatMessages.Num() - Number, 0);
	for (int32 i = FirstMessageIndex; i < ChatMessages.Num(); ++i)
	{
		LastMessages.Add(ChatMessages[i]);

		if (LastMessages.Num() >= Number)
		{
			break;
		}
	}

	return LastMessages;
}


void UChatRoom::JoinUser(const FString& UserId)
{
	JoinedUserIds.AddUnique(UserId);
}


void UChatRoom::LeaveUser(const FString& UserId)
{
	JoinedUserIds.Remove(UserId);
}


void UChatRoom::EnterUser(UChatComponent* ChatComponent)
{
	ActiveChatComponents.AddUnique(ChatComponent);
}


void UChatRoom::ExitUser(UChatComponent* ChatComponent)
{
	ActiveChatComponents.Remove(ChatComponent);
}


TArray<UChatComponent*> UChatRoom::GetActiveChatComponentsOfUser(FString UserId)
{
	TArray<UChatComponent*> Components;
	for (auto* Comp : ActiveChatComponents)
	{
		if (Comp->GetUserInfo().UserID==UserId)
		{
			Components.Add(Comp);
		}
	}
	return Components;
}
