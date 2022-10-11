// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "UploadFileRequest.h"

#include "NetworkUtils.h"


void UUploadFileRequest::FileToDataPackage(const FFileDataPackageInfo& PackageInfo, TArray<uint8>& Package)
{
	TArray<uint8> RoomIdByteArray;
	UNetworkUtils::StringToByteArray(PackageInfo.RoomId, RoomIdByteArray);

	TArray<uint8> UserIdByteArray;
	UNetworkUtils::StringToByteArray(PackageInfo.UserId, UserIdByteArray);

	TArray<uint8> FileNameByteArray;
	UNetworkUtils::StringToByteArray(PackageInfo.FileName, FileNameByteArray);

	const int32 PackageLength =
		DATA_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + UserIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileNameByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + PackageInfo.FileContent.Num();

	Package.SetNum(PackageLength, false);

	int32 Offset = 0;
	WritePayloadToPackage(DATA_SIZE_BIT_DEPTH, RoomIdByteArray, Package, Offset);
	WritePayloadToPackage(DATA_SIZE_BIT_DEPTH, UserIdByteArray, Package, Offset);
	WritePayloadToPackage(DATA_SIZE_BIT_DEPTH, FileNameByteArray, Package, Offset);
	WritePayloadToPackage(DATA_SIZE_BIT_DEPTH, PackageInfo.FileContent, Package, Offset);
}


void UUploadFileRequest::DataPackageToFile(const TArray<uint8>& Package, FFileDataPackageInfo& PackageInfo)
{
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> UserIdByteArray;
	TArray<uint8> FileNameByteArray;

	int32 Offset = 0;
	ReadPayloadFromPackage(DATA_SIZE_BIT_DEPTH, Package, RoomIdByteArray, Offset);
	ReadPayloadFromPackage(DATA_SIZE_BIT_DEPTH, Package, UserIdByteArray, Offset);
	ReadPayloadFromPackage(DATA_SIZE_BIT_DEPTH, Package, FileNameByteArray, Offset);
	ReadPayloadFromPackage(DATA_SIZE_BIT_DEPTH, Package, PackageInfo.FileContent, Offset);

	PackageInfo.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PackageInfo.UserId = UNetworkUtils::ByteArrayToString(UserIdByteArray);
	PackageInfo.FileName = UNetworkUtils::ByteArrayToString(FileNameByteArray);
}
