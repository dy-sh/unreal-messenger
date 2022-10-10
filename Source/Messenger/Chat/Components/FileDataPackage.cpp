// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileDataPackage.h"

#include "NetworkUtils.h"

constexpr int32 DATA_SIZE_BIT_DEPTH = 4;


void FileDataPackage::FileToDataPackage(const FFileDataPackageInfo& PackageInfo, TArray<uint8>& Package)
{
	TArray<uint8> RoomIdByteArray;
	UNetworkUtils::StringToByteArray(PackageInfo.RoomId, RoomIdByteArray);

	TArray<uint8> FileNameByteArray;
	UNetworkUtils::StringToByteArray(PackageInfo.FileName, FileNameByteArray);

	const int32 PackageLength =
		DATA_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileNameByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + PackageInfo.FileContent.Num();

	Package.SetNum(PackageLength, false);

	int32 Offset = 0;
	WritePayloadToPackage(DATA_SIZE_BIT_DEPTH, RoomIdByteArray, Package, Offset);
	WritePayloadToPackage(DATA_SIZE_BIT_DEPTH, FileNameByteArray, Package, Offset);
	WritePayloadToPackage(DATA_SIZE_BIT_DEPTH, PackageInfo.FileContent, Package, Offset);
}


void FileDataPackage::DataPackageToFile(const TArray<uint8>& Package, FFileDataPackageInfo& PackageInfo)
{
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> FileNameByteArray;

	int32 Offset = 0;
	ReadPayloadFromPackage(DATA_SIZE_BIT_DEPTH, Package, RoomIdByteArray, Offset);
	ReadPayloadFromPackage(DATA_SIZE_BIT_DEPTH, Package, FileNameByteArray, Offset);
	ReadPayloadFromPackage(DATA_SIZE_BIT_DEPTH, Package, PackageInfo.FileContent, Offset);

	PackageInfo.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PackageInfo.FileName = UNetworkUtils::ByteArrayToString(FileNameByteArray);
}


void FileDataPackage::WritePayloadToPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Payload,
	TArray<uint8>& Package, int32& Offset)
{
	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Package[i + Offset] = Payload.Num() >> i * 8;
	}
	Offset += DataSizeBitDepth;

	FMemory::Memcpy(Package.GetData() + Offset, Payload.GetData(), Payload.Num());
	Offset += Payload.Num();
}


void FileDataPackage::ReadPayloadFromPackage(const int32 DataSizeBitDepth, const TArray<uint8>& Package, TArray<uint8>& Payload,
	int32& Offset)
{
	int32 Size = 0;

	for (int i = 0; i < DataSizeBitDepth; ++i)
	{
		Size |= Package[i + Offset] << i * 8;
	}
	Offset += DataSizeBitDepth;

	Payload.SetNum(Size);
	FMemory::Memcpy(Payload.GetData(), Package.GetData() + Offset, Size);
	Offset += Size;
}
