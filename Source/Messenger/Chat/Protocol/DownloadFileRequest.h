// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.h"
#include "UObject/Object.h"
#include "DownloadFileRequest.generated.h"


USTRUCT(BlueprintType)
struct FDownloadFileRequestPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RoomId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileId;
};

UCLASS()
class MESSENGER_API UDownloadFileRequest : public UClientServerMessage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static UDownloadFileRequest* CreateDownloadFileRequest(const int32 MessageId, const FDownloadFileRequestPayload& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static const FDownloadFileRequestPayload& ParseDownloadFileRequestPayload(const TArray<uint8>& ByteArray);
	
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByPayload(const int32 MessageId, const FDownloadFileRequestPayload& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByByteArray(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	const FDownloadFileRequestPayload& GetPayload() const { return PayloadData; }

protected:
	FDownloadFileRequestPayload PayloadData;
};