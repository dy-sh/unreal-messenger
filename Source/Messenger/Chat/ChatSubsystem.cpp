// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatSubsystem.h"

#include "Messenger/Authorization/AuthorizationComponent.h"
#include "OpenSSLEncryption/OpenSSLEncryptionLibrary.h"
#include "Room/ChatRoom.h"
#include "User/ChatUser.h"


bool UChatSubsystem::CreateRoom(const FChatRoomSettings& Settings, FString& RoomId)
{
	RoomId = FGuid::NewGuid().ToString();

	if (ChatRooms.Contains(RoomId))
	{
		return false;
	}

	auto* Room = NewObject<UChatRoom>(this);

	if (!Room)
	{
		return false;
	}

	Room->SetRoomId(RoomId);
	Room->SetRoomSettings(Settings);

	ChatRooms.Add(RoomId, Room);

	return true;
}


UChatUser* UChatSubsystem::AddUser(UChatComponent* ChatComponent, UAuthorizationComponent* AuthorizationComponent)
{
	if (!ChatComponent) return nullptr;
	if (!AuthorizationComponent) return nullptr;
	if (AuthorizationComponent->GetClientAuthorizationState() != EAuthorizationState::Authorized) return nullptr;

	const FString ClientPublicKe = AuthorizationComponent->GetClientPublicKey();
	const FString UserId = UOpenSSLEncryptionLibrary::SHA1(ClientPublicKe);

	UChatUser* User;
	if (Users.Contains(UserId))
	{
		User = *Users.Find(UserId);
	}
	else
	{
		User = NewObject<UChatUser>(this);
		if (User)
		{
			Users.Add(UserId, User);
		}
	}

	if (!User) return nullptr;

	User->PrivateInfo.LastSeen = FDateTime::Now();
	User->ChatComponents.Add(ChatComponent);
	User->Info.UserID = UserId;
	User->Info.IsOnline = true;

	return User;
}


bool UChatSubsystem::RemoveUser(UChatComponent* ChatComponent, UAuthorizationComponent* AuthorizationComponent)
{
	if (!ChatComponent) return false;
	if (!AuthorizationComponent) return false;

	const FString ClientPublicKe = AuthorizationComponent->GetClientPublicKey();
	const FString UserId = UOpenSSLEncryptionLibrary::SHA1(ClientPublicKe);

	if (!Users.Contains(UserId)) return false;

	UChatUser* User = *Users.Find(UserId);

	User->ChatComponents.Remove(ChatComponent);

	return true;
}


UChatRoom* UChatSubsystem::GetRoom(const FString& RoomId) const
{
	return ChatRooms.Contains(RoomId) ? ChatRooms[RoomId] : nullptr;
}


UChatUser* UChatSubsystem::GetUser(const FString& UserId) const
{
	return Users.Contains(UserId) ? Users[UserId] : nullptr;
}
