// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.h"
#include "Messenger/Chat/ChatTypes.h"
#include "UObject/Object.h"
#include "UploadFileResponse.generated.h"


USTRUCT(BlueprintType)
struct FUploadFileResponsePayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSuccess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileId;
};

UCLASS(BlueprintType)
class MESSENGER_API UUploadFileResponse : public UClientServerMessage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static UUploadFileResponse* CreateUploadFileResponse(const FUploadFileResponsePayload& Payload);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static const FUploadFileResponsePayload& ParseUploadFileResponsePayload(const TArray<uint8>& ByteArray);
	
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByPayload(const FUploadFileResponsePayload& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByByteArray(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	const FUploadFileResponsePayload& GetPayload() const { return PayloadData; }

	virtual uint8 GetMessageType() const override{return (uint8)EClientServerMessageType::UploadFileResponse;}

protected:
	FUploadFileResponsePayload PayloadData;
};
