// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ClientServerMessage.h"
#include "UObject/Object.h"
#include "UploadFileRequest.generated.h"


USTRUCT()
struct FFileDataPackageInfo
{
	GENERATED_BODY();
	
	FString RoomId;
	FString UserId;
	FString FileName;
	TArray<uint8> FileContent;
};

UCLASS()
class MESSENGER_API UUploadFileRequest : public UClientServerMessage<FFileDataPackageInfo>
{
	GENERATED_BODY()

public:
	virtual void FileToDataPackage(const FFileDataPackageInfo& PackageInfo, TArray<uint8>& Package) override;
	virtual void DataPackageToFile(const TArray<uint8>& Package, FFileDataPackageInfo& PackageInfo) override;
};
