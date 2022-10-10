// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatWidget.h"

#include "Messenger/Authorization/AuthorizationComponent.h"
#include "Messenger/Chat/Components/ChatComponent.h"
#include "Messenger/Chat/Components/FileTransferComponent.h"
#include "Messenger/Chat/Core/ChatGameInstance.h"


bool UChatWidget::Initialize()
{
	if (GetWorld())
	{
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			if (auto* Comp = PlayerController->FindComponentByClass<UChatComponent>())
			{
				ChatComponent = Comp;
				ChatComponent->OnMessageReceived.AddDynamic(this, &ThisClass::OnMessageReceived);
			}

			if (auto* Comp = PlayerController->FindComponentByClass<URoomComponent>())
			{
				RoomComponent = Comp;
				RoomComponent->OnJoinedRoomsUpdated.AddDynamic(this, &ThisClass::OnJoinedRoomsUpdated);
				RoomComponent->OnJoinRoom.AddDynamic(this, &ThisClass::OnJoinRoom);
				RoomComponent->OnLeaveRoom.AddDynamic(this, &ThisClass::OnLeaveRoom);
				RoomComponent->OnEnterRoom.AddDynamic(this, &ThisClass::OnEnterRoom);
				RoomComponent->OnExitRoom.AddDynamic(this, &ThisClass::OnExitRoom);
			}

			if (auto* Comp = PlayerController->FindComponentByClass<UAuthorizationComponent>())
			{
				Comp->OnAuthorizationComplete.AddDynamic(this, &ThisClass::OnAuthorizationComplete);
				if (Comp->GetClientAuthorizationState() != EAuthorizationState::NotRequested) // if already requested
				{
					OnAuthorizationComplete(
						Comp->GetClientAuthorizationState(),
						Comp->GetClientEncryptionKeys(),
						Comp->GetServerEncryptionKeys());
				}
			}

			if (auto* Comp = PlayerController->FindComponentByClass<UFileTransferComponent>())
			{
				FileTransferComponent = Comp;
			}

			FInputModeUIOnly InputModeData;
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetShowMouseCursor(true);
			PlayerController->SetInputMode(InputModeData);
		}
	}

	return Super::Initialize();
}


void UChatWidget::CreateRoom(const FChatRoomSettings& Settings)
{
	if (RoomComponent)
	{
		RoomComponent->ServerCreateRoom(Settings);
	}
}


void UChatWidget::JoinRoom(const FString& RoomId)
{
	if (RoomComponent)
	{
		RoomComponent->ServerJoinRoom(RoomId);
	}
}

void UChatWidget::OnAuthorizationComplete(const EAuthorizationState State, const FEncryptionKeys&
	ClientEncryptionKeys, const FEncryptionKeys& ServerEncryptionKeys)
{
	if (State != EAuthorizationState::Authorized)
	{
		if (ChatComponent)
		{
			ChatComponent->DisconnectFromServer();
			return;
		}
	}

	if (const auto* GameInstance = Cast<UChatGameInstance>(GetGameInstance()))
	{
		const FChatSettings Settings = GameInstance->LoadSettings();

		if (ChatComponent)
		{
			ChatComponent->ServerChangeUserName(Settings.NickName); // todo remove
		}
	}
}


void UChatWidget::OnMessageReceived(const FChatMessage& Message)
{
	ShowNewMessage(Message);
}


void UChatWidget::OnJoinRoom(const FString& RoomId, const FUserInfo& User)
{
	FChatMessage Message;
	Message.Date = FDateTime::Now();
	Message.Text = User.UserName + " joined room.";
	ShowNewMessage(Message);
}


void UChatWidget::OnLeaveRoom(const FString& RoomId, const FUserInfo& User)
{
	FChatMessage Message;
	Message.Date = FDateTime::Now();
	Message.Text = User.UserName + " leave room.";
	ShowNewMessage(Message);
}


void UChatWidget::SendEncryptedMessage(const FString& Text)
{
	if (ChatComponent)
	{
		ChatComponent->SendEncryptedMessageToServer(Text);
	}
}


bool UChatWidget::SendFile(const FString& FileName)
{
	if (FileTransferComponent && RoomComponent)
	{		
		return FileTransferComponent->SendFile(RoomComponent->GetActiveRoomId(), FileName);
	}

	return false;
}




