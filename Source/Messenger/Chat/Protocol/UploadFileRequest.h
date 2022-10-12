// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.h"
#include "UploadFileRequest.generated.h"


USTRUCT(BlueprintType)
struct FFileDataPackageInfo
{
	GENERATED_BODY()
	;

	FString RoomId;
	FString UserId;
	FString FileName;
	TArray<uint8> FileContent;
};

UCLASS(BlueprintType)
class MESSENGER_API UUploadFileRequest : public UClientServerMessage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	virtual UUploadFileRequest* CreateUploadFileRequest(const FFileDataPackageInfo& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByPayload(const int32 MessageId, const FFileDataPackageInfo& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByByteArray(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	virtual const FFileDataPackageInfo& GetPayload() const { return PayloadData; }


protected:
	FFileDataPackageInfo PayloadData;
};
