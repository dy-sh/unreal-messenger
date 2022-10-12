// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.h"
#include "Messenger/Chat/ChatTypes.h"
#include "DownloadFileResponse.generated.h"



USTRUCT(BlueprintType)
struct FDownloadFileResponsePayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSuccess;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<uint8> FileContent;
};

UCLASS(BlueprintType)
class MESSENGER_API UDownloadFileResponse : public UClientServerMessage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static UDownloadFileResponse* CreateDownloadFileResponse(const FDownloadFileResponsePayload& Payload);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	static const FDownloadFileResponsePayload& ParseDownloadFileResponsePayload(const TArray<uint8>& ByteArray);
	
	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByPayload(const FDownloadFileResponsePayload& FileInfo);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	void InitializeByByteArray(const TArray<uint8>& ByteArray);

	UFUNCTION(BlueprintPure, Category = "ClientServerMessages")
	const FDownloadFileResponsePayload& GetPayload() const { return PayloadData; }

	virtual uint8 GetMessageType() const override{return (uint8)EClientServerMessageType::DownloadFileResponse;}

protected:
	FDownloadFileResponsePayload PayloadData;
};
