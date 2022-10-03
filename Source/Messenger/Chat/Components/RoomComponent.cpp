// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "RoomComponent.h"

#include "ChatComponent.h"
#include "ChatServerComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Messenger/Authorization/AuthorizationComponent.h"
#include "Messenger/Chat/Room/ChatRoom.h"
#include "Messenger/Chat/User/ChatUser.h"


void URoomComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;
	if (!GetOwner()) return;

	if (auto* GameMode = GetWorld()->GetAuthGameMode())
	{
		ChatServerComponent = GameMode->FindComponentByClass<UChatServerComponent>();
	}

	ChatComponent = GetOwner()->FindComponentByClass<UChatComponent>();

	AuthorizationComponent = GetOwner()->FindComponentByClass<UAuthorizationComponent>();
	if (AuthorizationComponent)
	{
		AuthorizationComponent->OnAuthorizationComplete.AddDynamic(this, &ThisClass::OnAuthorizationComplete);
		if (AuthorizationComponent->GetClientAuthorizationState() != EAuthorizationState::NotRequested)
		{
			OnAuthorizationComplete(
				AuthorizationComponent->GetClientAuthorizationState(),
				AuthorizationComponent->GetClientEncryptionKeys(),
				AuthorizationComponent->GetServerEncryptionKeys());
		}
	}
}


void URoomComponent::OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys& ClientKeys,
	const FEncryptionKeys& ServerKeys)
{
	if (State == EAuthorizationState::Authorized)
	{
		if (GetOwnerRole() == ENetRole::ROLE_Authority)
		{
			ServerGetJoinedChatRooms();
		}
	}
}


void URoomComponent::ServerGetJoinedChatRooms_Implementation()
{
	if (!ChatServerComponent) return;
	if (!ChatComponent) return;
	if (!ChatComponent->ChatUser) return;

	TArray<FString> RoomIds;
	TArray<FChatRoomSettings> RoomSettings;

	for (const FString& RoomId : ChatComponent->ChatUser->PrivateInfo.JoinedRoomIds)
	{
		if (const auto* Room = ChatServerComponent->GetRoom(RoomId))
		{
			RoomIds.Add(RoomId);
			RoomSettings.Add(Room->GetRoomSettings());
		}
	}

	ClientGetJoinedChatRooms(RoomIds, RoomSettings);
}


void URoomComponent::ClientGetJoinedChatRooms_Implementation(const TArray<FString>& RoomIds,
	const TArray<FChatRoomSettings>& RoomSettings)
{
	for (int i = 0; i < RoomIds.Num(); ++i)
	{
		if (RoomIds[i] == ActiveRoomId)
		{
			ActiveRoomSettings = RoomSettings[i];
		}
	}
	OnJoinedRoomsUpdated.Broadcast(ActiveRoomId, RoomIds, RoomSettings);
}


void URoomComponent::ServerCreateRoom_Implementation(const FChatRoomSettings& Settings)
{
	FString RoomId;
	if (ChatServerComponent->CreateRoom(Settings, RoomId))
	{
		ServerJoinRoom(RoomId);
	}
}


void URoomComponent::ServerJoinRoom_Implementation(const FString& RoomId)
{
	if (!ChatServerComponent) return;
	if (!ChatComponent) return;
	if (!ChatComponent->ChatUser) return;

	auto* Room = ChatServerComponent->GetRoom(RoomId);
	if (!Room) return;

	Room->JoinUser(ChatComponent->ChatUser->Info.UserID);

	ChatComponent->ChatUser->PrivateInfo.JoinedRoomIds.Add(RoomId);

	for (auto* ChatComp : ChatComponent->ChatUser->ChatComponents) 
	{	
		if (auto* RoomComp = ChatComp->GetRoomComponent())
		{
			RoomComp->ServerGetJoinedChatRooms();
		}		
	}
	

	for (auto* ChatComp : Room->GetActiveChatComponents())
	{
		if (auto* RoomComponent = ChatComp->GetRoomComponent())
		{
			RoomComponent->ClientJoinRoom(RoomId, ChatComponent->ChatUser->Info);
		}
	}
}


void URoomComponent::ClientJoinRoom_Implementation(const FString& RoomId, const FUserInfo& UserInfo)
{
	OnJoinRoom.Broadcast(RoomId, UserInfo);
}


void URoomComponent::ServerLeaveRoom_Implementation(const FString& RoomId)
{
	if (!ChatServerComponent) return;
	if (!ChatComponent) return;
	if (!ChatComponent->ChatUser) return;

	auto* Room = ChatServerComponent->GetRoom(RoomId);
	if (!Room) return;

	Room->LeaveUser(ChatComponent->ChatUser->Info.UserID);

	ChatComponent->ChatUser->PrivateInfo.JoinedRoomIds.Remove(RoomId);

	for (auto* ChatComp : ChatComponent->ChatUser->ChatComponents) 
	{		
		if (auto* RoomComp = ChatComp->GetRoomComponent())
		{
			if (RoomComp->GetActiveRoomId() == RoomId)
			{
				RoomComp->ServerExitRoom(RoomId);
			}
			RoomComp->ServerGetJoinedChatRooms();
		}		
	}

	for (auto* ChatComp : Room->GetActiveChatComponents())
	{
		if (auto* RoomComp = ChatComp->GetRoomComponent())
		{
			RoomComp->ClientLeaveRoom(RoomId, ChatComponent->ChatUser->Info);
		}
	}
}


void URoomComponent::ClientLeaveRoom_Implementation(const FString& RoomId, const FUserInfo& UserInfo)
{
	OnLeaveRoom.Broadcast(RoomId, UserInfo);
}


void URoomComponent::ServerEnterRoom_Implementation(const FString& RoomId)
{
	if (!ChatServerComponent) return;
	if (!ChatComponent) return;

	if (ChatServerComponent->GetRoom(ActiveRoomId))
	{
		ServerExitRoom(ActiveRoomId);
	}

	auto* Room = ChatServerComponent->GetRoom(RoomId);
	if (!Room) return;

	ActiveRoomId = RoomId;
	Room->EnterUser(ChatComponent);
	const TArray<FChatMessage>& Messages = Room->GetLastMessages(100);
	ClientEnterRoom(RoomId, Messages);

	ServerGetJoinedChatRooms(); // update room settings if changed
}


void URoomComponent::ClientEnterRoom_Implementation(const FString& RoomId, const TArray<FChatMessage>& Messages)
{
	ActiveRoomId = RoomId;
	OnEnterRoom.Broadcast(RoomId, Messages);
}


void URoomComponent::ServerExitRoom_Implementation(const FString& RoomId)
{
	if (!ChatServerComponent) return;
	if (!ChatComponent) return;

	auto* Room = ChatServerComponent->GetRoom(RoomId);
	if (!Room) return;

	Room->ExitUser(ChatComponent);
	ClientExitRoom(RoomId);
}


void URoomComponent::ClientExitRoom_Implementation(const FString& RoomId)
{
	ActiveRoomId = "";
	OnExitRoom.Broadcast(RoomId);
}


void URoomComponent::ServerEditRoomSettings_Implementation(const FString& RoomId, const FChatRoomSettings& Settings)
{
	if (!ChatServerComponent) return;
	if (!ChatComponent) return;

	auto* Room = ChatServerComponent->GetRoom(RoomId);
	if (!Room) return;

	Room->SetRoomSettings(Settings);

	for (auto* ChatComp : Room->GetActiveChatComponents())
	{
		if (auto* RoomComponent = ChatComp->GetRoomComponent())
		{
			RoomComponent->ServerGetJoinedChatRooms();
		}
	}
}
