// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "AuthorizationTypes.generated.h"

UENUM(BlueprintType)
enum class EAuthorizationState: uint8
{
	NotRequested,
	Failed,
	Authorized
};

USTRUCT(BlueprintType)
struct FEncryptionKeys
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PublicKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PrivateKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AesKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AesIvec;
};
