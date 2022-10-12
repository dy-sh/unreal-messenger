// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "DownloadFileResponse.h"
#include "NetworkUtils.h"
#include "Messenger/Chat/ChatTypes.h"


UDownloadFileResponse* UDownloadFileResponse::CreateDownloadFileResponse(const FDownloadFileResponsePayload& Payload)
{
	auto* Obj = NewObject<UDownloadFileResponse>();
	Obj->InitializeByPayload(Payload);
	return Obj;
}


const FDownloadFileResponsePayload& UDownloadFileResponse::ParseDownloadFileResponsePayload(const TArray<uint8>& ByteArray)
{
	auto* Obj = NewObject<UDownloadFileResponse>();
	Obj->InitializeByByteArray(ByteArray);
	return Obj->GetPayload();
}


void UDownloadFileResponse::InitializeByPayload(const FDownloadFileResponsePayload& FileInfo)
{
	MessType = (int32)EClientServerMessageType::DownloadFileResponse;
	PayloadData = FileInfo;

	TArray<uint8> RoomIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.RoomId, RoomIdByteArray);

	TArray<uint8> UserIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.UserId, UserIdByteArray);

	TArray<uint8> FileNameByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileName, FileNameByteArray);

	TArray<uint8> FileIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileId, FileIdByteArray);

	const int32 Length =
		DATA_SIZE_BIT_DEPTH + //MessageType
		DATA_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + UserIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileNameByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileIdByteArray.Num() +
		DATA_SIZE_BIT_DEPTH + FileInfo.FileContent.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, MessType, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, RoomIdByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, UserIdByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileNameByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileIdByteArray, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileInfo.FileContent, DataByteArray, Offset);
}


void UDownloadFileResponse::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageType;
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> UserIdByteArray;
	TArray<uint8> FileNameByteArray;
	TArray<uint8> FileIdByteArray;

	int32 Offset = 0;
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, MessageType, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, RoomIdByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, UserIdByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, FileNameByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, FileIdByteArray, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, PayloadData.FileContent, Offset);

	PayloadData.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PayloadData.UserId = UNetworkUtils::ByteArrayToString(UserIdByteArray);
	PayloadData.FileName = UNetworkUtils::ByteArrayToString(FileNameByteArray);
	PayloadData.FileId = UNetworkUtils::ByteArrayToString(FileIdByteArray);
}
