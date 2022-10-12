// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatMessageWidget.generated.h"


UCLASS()
class MESSENGER_API UChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true), Category="FileInfo")
	FChatMessage Message;
};
