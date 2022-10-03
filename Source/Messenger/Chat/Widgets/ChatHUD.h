// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatHUD.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API AChatHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Widgets)
	TSubclassOf<class UChatWidget> ChatWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Widgets)
	TSubclassOf<class UChatMessageWidget> ChatMessageWidgetClass;

	UPROPERTY()
	UChatWidget* ChatWidget;

	UPROPERTY()
	UChatMessageWidget* ChatMessageWidget;

	void CreateChatUI();
};
