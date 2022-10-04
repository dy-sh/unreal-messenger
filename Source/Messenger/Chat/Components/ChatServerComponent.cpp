// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatServerComponent.h"
#include "ChatComponent.h"
#include "Messenger/Authorization/AuthorizationComponent.h"
#include "Messenger/Chat/Room/ChatRoom.h"
#include "Messenger/Chat/User/ChatUser.h"
#include "OpenSSLEncryption/OpenSSLEncryptionLibrary.h"


bool UChatServerComponent::CreateRoom(const FChatRoomSettings& Settings, FString& RoomId)
{
	RoomId = FGuid::NewGuid().ToString();

	if (ChatRooms.Contains(RoomId))
	{
		return false;
	}

	auto* Room = NewObject<UChatRoom>(GetOwner());

	if (!Room)
	{
		return false;
	}

	Room->SetRoomId(RoomId);
	Room->SetRoomSettings(Settings);

	ChatRooms.Add(RoomId, Room);

	return true;
}


UChatUser* UChatServerComponent::AddUser(UChatComponent* ChatComponent, UAuthorizationComponent* AuthorizationComponent)
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
		User = NewObject<UChatUser>(GetOwner());
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


bool UChatServerComponent::RemoveUser(UChatComponent* ChatComponent, UAuthorizationComponent* AuthorizationComponent)
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


UChatRoom* UChatServerComponent::GetRoom(const FString& RoomId) const
{
	return ChatRooms.Contains(RoomId) ? ChatRooms[RoomId] : nullptr;
}


UChatUser* UChatServerComponent::GetUser(const FString& UserId) const
{
	return Users.Contains(UserId) ? Users[UserId] : nullptr;
}
