// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "ChatComponent.h"


#include "FileTransferComponent.h"
#include "RoomComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Messenger/Authorization/AuthorizationComponent.h"
#include "Messenger/Chat/ChatSubsystem.h"
#include "OpenSSLEncryption/OpenSSLEncryptionLibrary.h"
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

	if (GetOwner()->HasAuthority())
	{
		ChatSubsystem = GetWorld()->GetSubsystem<UChatSubsystem>();
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
	FileTransferComponent = GetOwner()->FindComponentByClass<UFileTransferComponent>();
}


void UChatComponent::OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys& ClientKeys,
	const FEncryptionKeys& ServerKeys)
{
	if (State == EAuthorizationState::Authorized)
	{
		if (ChatSubsystem) // has authority
		{
			ChatUser = ChatSubsystem->AddUser(this, AuthorizationComponent);
			ClientSetUserInfo(ChatUser->Info, ChatUser->PrivateInfo);
		}
	}
}


void UChatComponent::DisconnectFromServer()
{
	if (!GetWorld()) return;
	if (!GetWorld()->GetGameInstance()) return;

	GetWorld()->GetGameInstance()->ReturnToMainMenu();
}


void UChatComponent::ServerGetUserInfo_Implementation()
{
	if (ChatUser)
	{
		ClientSetUserInfo(ChatUser->Info, ChatUser->PrivateInfo);
	}
}


void UChatComponent::ClientSetUserInfo_Implementation(const FUserInfo& Info, const FUserPrivateInfo& PrivateInfo)
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
	if (!UOpenSSLEncryptionLibrary::EncryptAes(Text, Keys.AesKey, Keys.AesIvec, EncryptedData,
		PayloadSize))
	{
		UE_LOG(LogChatComponent, Error, TEXT("Faild to encrypt client message"))
		return;
	}

	ServerSendEncryptedMessage(EncryptedData, PayloadSize);
}


void UChatComponent::ClientReceiveFileInfo_Implementation(const FTransferredFileInfo& FileInfo)
{
	UE_LOG(LogChatComponent, Warning, TEXT("Client received file info: %s"), *FileInfo.FileName);

	OnFileInfoReceived.Broadcast(FileInfo);
}


void UChatComponent::ServerSendEncryptedMessage_Implementation(const FString& EncryptedText, const int32 PayloadSize)
{
	UE_LOG(LogChatComponent, Display, TEXT("Server received encrypted message"));

	if (!AuthorizationComponent) return;

	const FEncryptionKeys Keys = AuthorizationComponent->GetServerEncryptionKeys();

	FString DecryptedText;
	if (!UOpenSSLEncryptionLibrary::DecryptAes(EncryptedText, PayloadSize, Keys.AesKey, Keys.AesIvec, DecryptedText))
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
	if (!UOpenSSLEncryptionLibrary::DecryptAes(EncryptedMessage.Text, PayloadSize, Keys.AesKey, Keys.AesIvec, DecryptedText))
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
	if (!ChatSubsystem) return;
	if (!AuthorizationComponent) return;
	if (!RoomComponent) return;

	auto* Room = ChatSubsystem->GetRoom(RoomComponent->GetActiveRoomId());
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
			if (!UOpenSSLEncryptionLibrary::EncryptAes(Message.Text, Keys.AesKey, Keys.AesIvec, EncryptedData,
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
