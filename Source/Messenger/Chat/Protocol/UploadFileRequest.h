// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.h"
#include "Messenger/Chat/ChatTypes.h"
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
	FString UserName;

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
	static UUploadFileRequest* CreateUploadFileRequest(const FUploadFileRequestPayload& Payload);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static const FUploadFileRequestPayload& ParseUploadFileRequestPayload(const TArray<uint8>& ByteArray);
	
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByPayload(const FUploadFileRequestPayload& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByByteArray(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	const FUploadFileRequestPayload& GetPayload() const { return PayloadData; }

	virtual uint8 GetMessageType() const override{return (uint8)EClientServerMessageType::UploadFileRequest;}

protected:
	FUploadFileRequestPayload PayloadData;
};
