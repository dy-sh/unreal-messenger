// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Messenger/Chat/ChatTypes.h"
#include "FileInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API UFileInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true), Category="FileInfo")
	FTransferredFileInfo FileInfo;

	UFUNCTION(BlueprintCallable)
	void ReceiveFile();
};
