// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ChatComponent.h"

#include "ChatServerComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Messenger/Authorization/AuthorizationComponent.h"
#include "OpenSSLEncryption/OpenSSLEncryptionBPLibrary.h"
#include "Messenger/Chat/Room/ChatRoom.h"
#include "Messenger/Chat/User/ChatUser.h"


DEFINE_LOG_CATEGORY_STATIC(LogChatComponent, All, All);


UChatComponent::UChatComponent()
{
	SetIsReplicatedByDefault(true);
}


void UChatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;
	if (!GetOwner()) return;

	if (auto* GameMode = GetWorld()->GetAuthGameMode())
	{
		ChatServerComponent = GameMode->FindComponentByClass<UChatServerComponent>();
	}

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

	RoomComponent = GetOwner()->FindComponentByClass<URoomComponent>();
}


void UChatComponent::OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys& ClientKeys,
	const FEncryptionKeys& ServerKeys)
{
	if (State == EAuthorizationState::Authorized)
	{
		if (ChatServerComponent) // has authority
		{
			ChatUser = ChatServerComponent->AddUser(this, AuthorizationComponent);
			ClientGetUserInfo(ChatUser->Info, ChatUser->PrivateInfo);
		}
	}
}


void UChatComponent::ServerGetUserInfo_Implementation()
{
	if (ChatUser)
	{
		ClientGetUserInfo(ChatUser->Info, ChatUser->PrivateInfo);
	}
}


void UChatComponent::ClientGetUserInfo_Implementation(const FUserInfo& Info, const FUserPrivateInfo& PrivateInfo)
{
	UserInfo = Info;
	UserPrivateInfo = PrivateInfo;
}


void UChatComponent::ServerSendMessage_Implementation(const FString& Text)
{
	UE_LOG(LogChatComponent, Warning, TEXT("Server received: %s"), *Text);

	SendMessageToAllUsersInRoom(Text, false);
}


void UChatComponent::ClientReceiveMessage_Implementation(const FChatMessage& Message)
{
	UE_LOG(LogChatComponent, Warning, TEXT("Client received: %s"), *Message.Text);

	OnMessageReceived.Broadcast(Message);
}


void UChatComponent::SendEncryptedMessageToServer(const FString& Text)
{
	if (!AuthorizationComponent) return;

	const FEncryptionKeys Keys = AuthorizationComponent->GetServerEncryptionKeys();

	FString EncryptedData;
	int32 PayloadSize;
	if (!UOpenSSLEncryptionBPLibrary::EncryptAes(Text, Keys.AesKey, Keys.AesIvec, EncryptedData,
		PayloadSize))
	{
		UE_LOG(LogChatComponent, Error, TEXT("Faild to encrypt client message"))
		return;
	}

	ServerSendEncryptedMessage(EncryptedData, PayloadSize);
}


void UChatComponent::ServerSendEncryptedMessage_Implementation(const FString& EncryptedText, const int32 PayloadSize)
{
	UE_LOG(LogChatComponent, Display, TEXT("Server received encrypted message"));

	if (!AuthorizationComponent) return;

	const FEncryptionKeys Keys = AuthorizationComponent->GetServerEncryptionKeys();

	FString DecryptedText;
	if (!UOpenSSLEncryptionBPLibrary::DecryptAes(EncryptedText, PayloadSize, Keys.AesKey, Keys.AesIvec, DecryptedText))
	{
		UE_LOG(LogChatComponent, Error, TEXT("Faild to decrypt client message"))
		return;
	}

	SendMessageToAllUsersInRoom(DecryptedText, true);
}


void UChatComponent::ClientReceiveEncryptedMessage_Implementation(const FChatMessage& EncryptedMessage,
	const int32 PayloadSize)
{
	UE_LOG(LogChatComponent, Display, TEXT("Client received encrypted message"));

	if (!AuthorizationComponent) return;

	const FEncryptionKeys Keys = AuthorizationComponent->GetClientEncryptionKeys();

	FString DecryptedText;
	if (!UOpenSSLEncryptionBPLibrary::DecryptAes(EncryptedMessage.Text, PayloadSize, Keys.AesKey, Keys.AesIvec, DecryptedText))
	{
		UE_LOG(LogChatComponent, Error, TEXT("Faild to decrypt client message"))
		return;
	}
	FChatMessage DecryptedMessage = EncryptedMessage;
	DecryptedMessage.Text = DecryptedText;
	OnMessageReceived.Broadcast(DecryptedMessage);
}


void UChatComponent::ServerChangeUserName_Implementation(const FString& NewName)
{
	UE_LOG(LogChatComponent, Warning, TEXT("Server change user name: %s"), *NewName);

	if (ChatUser)
	{
		ChatUser->Info.UserName = NewName;
	}
}


void UChatComponent::SendMessageToAllUsersInRoom(const FString& Text, const bool SendEncrypted)
{
	if (!ChatUser) return;
	if (!ChatServerComponent) return;
	if (!AuthorizationComponent) return;
	if (!RoomComponent) return;

	auto* Room = ChatServerComponent->GetRoom(RoomComponent->GetActiveRoomId());
	if (!Room) return;

	FChatMessage Message;
	Message.Text = Text;
	Message.Date = FDateTime::Now();
	Message.UserID = ChatUser->Info.UserID;
	Message.UserName = ChatUser->Info.UserName;

	Room->AddMessage(Message);

	for (auto* ChatComponent : Room->GetActiveChatComponents())
	{
		if (SendEncrypted)
		{
			if (!ChatComponent->AuthorizationComponent) return;
			
			const FEncryptionKeys Keys = ChatComponent->AuthorizationComponent->GetClientEncryptionKeys();

			FChatMessage EncryptedMessage = Message;
			FString EncryptedData;
			int32 PayloadSize;
			if (!UOpenSSLEncryptionBPLibrary::EncryptAes(Message.Text, Keys.AesKey, Keys.AesIvec, EncryptedData,
				PayloadSize))
			{
				UE_LOG(LogChatComponent, Error, TEXT("Faild to encrypt server message"))
				return;
			}

			EncryptedMessage.Text = EncryptedData;
			ChatComponent->ClientReceiveEncryptedMessage(EncryptedMessage, PayloadSize);
		}
		else
		{
			ChatComponent->ClientReceiveMessage(Message);
		}
	}
}
