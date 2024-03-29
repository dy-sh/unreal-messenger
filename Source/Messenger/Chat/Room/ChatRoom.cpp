﻿// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatRoom.h"

#include "Messenger/Chat/Components/ChatComponent.h"


TArray<FChatMessage> UChatRoom::GetLastMessages(const int32 Number)
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


bool UChatRoom::GetFileInfo(const FString& FileId, FTransferredFileInfo& FileInfo)
{
	for (const auto Info : FileInfos)
	{
		if (Info.FileId == FileId)
		{
			FileInfo = Info;
			return true;
		}
	}
	return false;
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


TArray<UChatComponent*> UChatRoom::GetActiveChatComponentsOfUser(FString UserId) const
{
	TArray<UChatComponent*> Components;
	for (auto* Comp : ActiveChatComponents)
	{
		if (Comp->GetUserInfo().UserID == UserId)
		{
			Components.Add(Comp);
		}
	}
	return Components;
}


TArray<UFileTransferComponent*> UChatRoom::GetActiveFileTransferComponentsOfUser(FString UserId) const
{
	TArray<UFileTransferComponent*> Components;
	for (const auto* ChatComp : ActiveChatComponents)
	{
		if (ChatComp->GetUserInfo().UserID == UserId)
		{
			if (auto* FileComp = ChatComp->GetFileTransferComponent())
			{
				Components.Add(FileComp);
			}
		}
	}
	return Components;
}
