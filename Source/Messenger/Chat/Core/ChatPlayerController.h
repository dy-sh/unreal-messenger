// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ChatPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class MESSENGER_API AChatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AChatPlayerController();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAuthorizationComponent* AuthorizationComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UChatComponent* ChatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URoomComponent* ChatRoomComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UFileTransferComponent* FileTransferComponent;
};
