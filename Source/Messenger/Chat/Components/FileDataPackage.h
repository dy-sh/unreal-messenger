// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FileDataPackage.generated.h"


USTRUCT()
struct FFileDataPackageInfo
{
	GENERATED_BODY();
	
	FString RoomId;
	FString FileName;
	TArray<uint8> FileContent;
};


class FileDataPackage
{
public:
	static void FileToDataPackage(const FFileDataPackageInfo& PackageInfo, TArray<uint8>& Package);
	static void DataPackageToFile(const TArray<uint8>& Package, FFileDataPackageInfo& PackageInfo);
private:
	static void WritePayloadToPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Payload, TArray<uint8>& Package, int32& Offset);
	static void ReadPayloadFromPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Package, TArray<uint8>& Payload, int32& Offset);

};
