// // Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "UploadFileRequest.h"
#include "NetworkUtils.h"


UUploadFileRequest* UUploadFileRequest::CreateUploadFileRequest(const int32 MessageId, const FFileDataPackageInfo& FileInfo)
{
	auto* Obj = NewObject<UUploadFileRequest>();
	Obj->InitializeByPayload(MessageId, FileInfo);
	return Obj;
}


const FFileDataPackageInfo& UUploadFileRequest::ParseByteArrayToFileRequestPayload(const TArray<uint8>& ByteArray)
{
	auto* Obj = NewObject<UUploadFileRequest>();
	Obj->InitializeByByteArray(ByteArray);
	return Obj->GetPayload();
}


void UUploadFileRequest::InitializeByPayload(const int32 MessageId, const FFileDataPackageInfo& FileInfo)
{
	MessId = MessageId;
	PayloadData = FileInfo;

	TArray<uint8> RoomIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.RoomId, RoomIdByteArray);

	TArray<uint8> UserIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.UserId, UserIdByteArray);

	TArray<uint8> FileNameByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileName, FileNameByteArray);

	const int32 Length =
		DATA_SIZE_BIT_DEPTH + //MessageId
		DATA_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + UserIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileNameByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileInfo.FileContent.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, MessageId, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, RoomIdByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, UserIdByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileNameByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileInfo.FileContent, DataByteArray, Offset);
}


void UUploadFileRequest::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageId;
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> UserIdByteArray;
	TArray<uint8> FileNameByteArray;

	int32 Offset = 0;
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, MessageId, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, RoomIdByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, UserIdByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, FileNameByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, PayloadData.FileContent, Offset);

	PayloadData.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PayloadData.UserId = UNetworkUtils::ByteArrayToString(UserIdByteArray);
	PayloadData.FileName = UNetworkUtils::ByteArrayToString(FileNameByteArray);
}
