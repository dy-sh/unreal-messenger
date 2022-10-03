// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ChatTypes.generated.h"


USTRUCT(BlueprintType)
struct FUserInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOnline;
};

USTRUCT(BlueprintType)
struct FUserPrivateInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> JoinedRoomIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime LastSeen;
};


USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime Date;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserName;
};

USTRUCT(BlueprintType)
struct FChatRoomSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RoomName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool AutoAcceptUsers;
};

USTRUCT(BlueprintType)
struct FChatSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString NickName;
};


