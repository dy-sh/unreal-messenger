// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "SettingsWidget.h"

#include "Messenger/Chat/Components/ChatComponent.h"
#include "Messenger/Chat/Core/ChatGameInstance.h"


void USettingsWidget::SaveSettings(const FChatSettings& Settings)
{
	FChatSettings OldSettings = LoadSettings();

	if (OldSettings.NickName != Settings.NickName)
	{
		if (auto* Controller = GetOwningPlayer())
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


FChatSettings USettingsWidget::LoadSettings()
{
	if (auto* GameInstance = Cast<UChatGameInstance>(GetGameInstance()))
	{
		return GameInstance->LoadSettings();
	}

	return FChatSettings{};
}
