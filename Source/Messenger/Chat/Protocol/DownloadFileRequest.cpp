// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "DownloadFileRequest.h"

#include "NetworkUtils.h"


UDownloadFileRequest* UDownloadFileRequest::CreateDownloadFileRequest(const int32 MessageId, const FDownloadFileRequestPayload& FileInfo)
{
	auto* Obj = NewObject<UDownloadFileRequest>();
	Obj->InitializeByPayload(MessageId, FileInfo);
	return Obj;
}


const FDownloadFileRequestPayload& UDownloadFileRequest::ParseDownloadFileRequestPayload(const TArray<uint8>& ByteArray)
{
	auto* Obj = NewObject<UDownloadFileRequest>();
	Obj->InitializeByByteArray(ByteArray);
	return Obj->GetPayload();
}


void UDownloadFileRequest::InitializeByPayload(const int32 MessageId, const FDownloadFileRequestPayload& FileInfo)
{
	MessId = MessageId;
	PayloadData = FileInfo;

	TArray<uint8> RoomIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.RoomId, RoomIdByteArray);

	TArray<uint8> UserIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.UserId, UserIdByteArray);

	TArray<uint8> FileIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileId, FileIdByteArray);

	const int32 Length =
		DATA_SIZE_BIT_DEPTH + //MessageId
		DATA_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + UserIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileIdByteArray.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, MessageId, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, RoomIdByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, UserIdByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileIdByteArray, DataByteArray, Offset);
}


void UDownloadFileRequest::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageId;
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> UserIdByteArray;
	TArray<uint8> FileIdByteArray;

	int32 Offset = 0;
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, MessageId, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, RoomIdByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, UserIdByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, FileIdByteArray, Offset);

	PayloadData.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PayloadData.UserId = UNetworkUtils::ByteArrayToString(UserIdByteArray);
	PayloadData.FileId = UNetworkUtils::ByteArrayToString(FileIdByteArray);
}