// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Messenger/Chat/Settings/ChatSettingsSave.h"


FChatSettings UChatGameInstance::LoadSettings() const
{
	auto* Save = Cast<UChatSettingsSave>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0));

	if (!Save)
	{
		Save = Cast<UChatSettingsSave>(UGameplayStatics::CreateSaveGameObject(UChatSettingsSave::StaticClass()));
	}

	return Save->Settings;
}


void UChatGameInstance::SaveSettings(const FChatSettings& Settings)
{
	auto* Save = Cast<UChatSettingsSave>(UGameplayStatics::CreateSaveGameObject(UChatSettingsSave::StaticClass()));
	Save->Settings = Settings;

	if (!UGameplayStatics::SaveGameToSlot(Save, "SaveSlot", 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Cant save settings to file"));
	}
}
