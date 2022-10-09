// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChatGameMode.generated.h"

class UChatServerComponent;
class UFileTransferServerComponent;

UCLASS()
class MESSENGER_API AChatGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChatServerComponent* ChatServerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFileTransferServerComponent* FileTransferServerComponent;
	
	AChatGameMode();
	
};
