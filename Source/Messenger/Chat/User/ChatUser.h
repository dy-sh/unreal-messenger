// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Messenger/Chat/ChatTypes.h"
#include "UObject/Object.h"
#include "ChatUser.generated.h"

/**
 * 
 */
UCLASS()
class MESSENGER_API UChatUser : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUserInfo Info;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUserPrivateInfo PrivateInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UChatComponent*> ChatComponents;
};
