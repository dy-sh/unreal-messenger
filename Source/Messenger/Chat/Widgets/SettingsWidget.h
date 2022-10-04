// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Messenger/Chat/ChatTypes.h"
#include "SettingsWidget.generated.h"





UCLASS()
class MESSENGER_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="Settings")
	void SaveSettings(const FChatSettings& Settings);

	UFUNCTION(BlueprintCallable, Category="Settings")
	FChatSettings LoadSettings() const;

	UFUNCTION(BlueprintCallable, Category="Settings")
	void DisconnectFromServer();
};
