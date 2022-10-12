// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ChatMessageWidget.h"
#include "Messenger/Chat/ChatTypes.h"
#include "ChatMessageFileWidget.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API UChatMessageFileWidget : public UChatMessageWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true), Category="FileInfo")
	FTransferredFileInfo FileInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn=true), Category="FileInfo")
	bool bSuccess;
	
	UFUNCTION(BlueprintCallable, Category="FileInfo")
	void DownloadFileFromServer();

	UFUNCTION(BlueprintCallable, Category="FileInfo")
	void SaveDownloadedFile(FString Path);
};
