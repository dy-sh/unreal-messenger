// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatHUD.h"
#include "ChatWidget.h"
#include "Blueprint/UserWidget.h"


void AChatHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateChatUI();
}


void AChatHUD::CreateChatUI()
{
	ChatWidget = CreateWidget<UChatWidget>(GetOwningPlayerController(), ChatWidgetClass);
	if (ChatWidget)
	{
		ChatWidget->AddToViewport();
	}
}
