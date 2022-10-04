// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "SettingsWidget.h"

#include "Messenger/Chat/Components/ChatComponent.h"
#include "Messenger/Chat/Core/ChatGameInstance.h"


void USettingsWidget::SaveSettings(const FChatSettings& Settings)
{
	const FChatSettings OldSettings = LoadSettings();

	if (OldSettings.NickName != Settings.NickName)
	{
		if (const auto* Controller = GetOwningPlayer())
		{
			if (auto* ChatComponent = Controller->FindComponentByClass<UChatComponent>())
			{
				ChatComponent->ServerChangeUserName(Settings.NickName);
			}
		}
	}

	if (auto* GameInstance = Cast<UChatGameInstance>(GetGameInstance()))
	{
		GameInstance->SaveSettings(Settings);
	}
}


FChatSettings USettingsWidget::LoadSettings() const
{
	if (const auto* GameInstance = Cast<UChatGameInstance>(GetGameInstance()))
	{
		return GameInstance->LoadSettings();
	}

	return FChatSettings{};
}


void USettingsWidget::DisconnectFromServer()
{
	if (const auto* Controller = GetOwningPlayer())
	{
		if (auto* ChatComponent = Controller->FindComponentByClass<UChatComponent>())
		{
			ChatComponent->DisconnectFromServer();
		}
	}
}
