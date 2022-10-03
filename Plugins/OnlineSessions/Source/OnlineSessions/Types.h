// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Types.generated.h"

#define MATCH_TYPE FName("MatchType")


USTRUCT(BlueprintType)
struct FSessionParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPublicConnections{20};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MatchType{"NoType"};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PathToServerTravel{"/OnlineSessions/Example/LobbyExample"};
};
