// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ConnectionWidget.h"

#include "ConnectionSettingsSave.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessions/OnlineSessionsSubsystem.h"


bool UConnectionWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (StartServerButton)
	{
		StartServerButton->OnClicked.AddDynamic(this, &ThisClass::OnStartServerButtonClicked);
	}

	if (FindServerButton)
	{
		FindServerButton->OnClicked.AddDynamic(this, &ThisClass::OnFindServerButtonClicked);
	}

	if (ConnectToIPButton)
	{
		ConnectToIPButton->OnClicked.AddDynamic(this, &ThisClass::OnConnectToIPButtonClicked);
	}

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		OnlineSessionsSubsystem = GameInstance->GetSubsystem<UOnlineSessionsSubsystem>();

		if (OnlineSessionsSubsystem)
		{
			OnlineSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
			OnlineSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
			OnlineSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
			OnlineSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
			OnlineSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		}
	}

	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetShowMouseCursor(true);
			PlayerController->SetInputMode(InputModeData);
		}
	}

	const UConnectionSettingsSave* Settings = LoadSettings();
	if (Settings && ServerIPTextBox)
	{
		ServerIPTextBox->SetText(FText::FromString(Settings->ServerIP));
	}

	return true;
}


void UConnectionWidget::Configure(const FSessionParams& Params)
{
	SessionParams = Params;
}


void UConnectionWidget::EnableButtons(bool bEnable)
{
	if (StartServerButton)
	{
		StartServerButton->SetIsEnabled(bEnable);
	}
	if (ConnectToIPButton)
	{
		ConnectToIPButton->SetIsEnabled(bEnable);
	}
	if (FindServerButton)
	{
		FindServerButton->SetIsEnabled(bEnable);
	}
}


void UConnectionWidget::OnStartServerButtonClicked()
{
	if (OnlineSessionsSubsystem)
	{
		EnableButtons(false);
		OnlineSessionsSubsystem->CreateSession(SessionParams);
	}
}


void UConnectionWidget::OnFindServerButtonClicked()
{
	if (OnlineSessionsSubsystem)
	{
		EnableButtons(false);
		OnlineSessionsSubsystem->FindSessions();
	}
}


void UConnectionWidget::OnConnectToIPButtonClicked()
{
	if (!ServerIPTextBox) return;
	if (!GetWorld()) return;

	const FString ServerIP = ServerIPTextBox->Text.ToString();

	if (UConnectionSettingsSave* Settings = LoadSettings())
	{
		Settings->ServerIP = ServerIP;
		SaveSettings(Settings);
	}

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->ClientTravel(ServerIP, TRAVEL_Absolute);
	}
}


void UConnectionWidget::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (UWorld* World = GetWorld())
		{
			const FString PathToTravel = FString::Printf(TEXT("%s?listen"), *SessionParams.PathToServerTravel);
			World->ServerTravel(PathToTravel);
		}
	}
	else
	{
		EnableButtons(true);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
				FString(TEXT("Failed to create session!"))
				);
		}
	}
}


void UConnectionWidget::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (!OnlineSessionsSubsystem || !bWasSuccessful || SessionResults.Num() == 0)
	{
		EnableButtons(true);
		return;
	}

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(MATCH_TYPE, SettingsValue);
		if (SettingsValue == SessionParams.MatchType)
		{
			OnlineSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	EnableButtons(true);
}


void UConnectionWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		const IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
				return;
			}
		}
	}

	EnableButtons(true);
}


void UConnectionWidget::OnDestroySession(bool bWasSuccessful)
{
}


void UConnectionWidget::OnStartSession(bool bWasSuccessful)
{
}


UConnectionSettingsSave* UConnectionWidget::LoadSettings() const
{
	auto* Save = Cast<UConnectionSettingsSave>(UGameplayStatics::LoadGameFromSlot("ConnectionSettings", 0));

	if (!Save)
	{
		Save = Cast<UConnectionSettingsSave>(
			UGameplayStatics::CreateSaveGameObject(UConnectionSettingsSave::StaticClass()));
		UGameplayStatics::SaveGameToSlot(Save, "ConnectionSettings", 0);
	}

	return Save;
}


void UConnectionWidget::SaveSettings(UConnectionSettingsSave* Save) const
{
	UGameplayStatics::SaveGameToSlot(Save, "ConnectionSettings", 0);
}
