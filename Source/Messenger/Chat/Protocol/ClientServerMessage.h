// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FileDataPackage.generated.h"

constexpr int32 DATA_SIZE_BIT_DEPTH = 4;


template<class T>  
UCLASS()
class UClientServerMessage: public UObject
{
	GENERATED_BODY()
	
public:
	virtual void FileToDataPackage(const T& PackageInfo, TArray<uint8>& Package)= 0;
	virtual void DataPackageToFile(const TArray<uint8>& Package, T& PackageInfo)= 0;
protected:
	void WritePayloadToPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Payload, TArray<uint8>& Package, int32& Offset);
	void ReadPayloadFromPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Package, TArray<uint8>& Payload, int32& Offset);
};

