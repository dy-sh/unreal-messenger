// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.h"
#include "UploadFileRequest.generated.h"


USTRUCT(BlueprintType)
struct FUploadFileRequestPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RoomId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<uint8> FileContent;
};

UCLASS(BlueprintType)
class MESSENGER_API UUploadFileRequest : public UClientServerMessage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static UUploadFileRequest* CreateUploadFileRequest(const int32 MessageId, const FUploadFileRequestPayload& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static const FUploadFileRequestPayload& ParseUploadFileRequestPayload(const TArray<uint8>& ByteArray);
	
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByPayload(const int32 MessageId, const FUploadFileRequestPayload& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByByteArray(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	const FUploadFileRequestPayload& GetPayload() const { return PayloadData; }

protected:
	FUploadFileRequestPayload PayloadData;
};
