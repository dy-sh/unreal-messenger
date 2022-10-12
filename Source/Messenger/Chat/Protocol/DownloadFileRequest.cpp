// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "DownloadFileRequest.h"

#include "NetworkUtils.h"
#include "Messenger/Chat/ChatTypes.h"


UDownloadFileRequest* UDownloadFileRequest::CreateDownloadFileRequest(const FDownloadFileRequestPayload& Payload)
{
	auto* Obj = NewObject<UDownloadFileRequest>();
	Obj->InitializeByPayload(Payload);
	return Obj;
}


const FDownloadFileRequestPayload& UDownloadFileRequest::ParseDownloadFileRequestPayload(const TArray<uint8>& ByteArray)
{
	auto* Obj = NewObject<UDownloadFileRequest>();
	Obj->InitializeByByteArray(ByteArray);
	return Obj->GetPayload();
}


void UDownloadFileRequest::InitializeByPayload(const FDownloadFileRequestPayload& FileInfo)
{
	MessType = (int32) EClientServerMessageType::DownloadFileRequest;
	PayloadData = FileInfo;

	TArray<uint8> RoomIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.RoomId, RoomIdByteArray);

	TArray<uint8> UserIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.UserId, UserIdByteArray);

	TArray<uint8> FileIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileId, FileIdByteArray);

	const int32 Length =
		1 + //MessageType
		BYTE_ARRAY_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + UserIdByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + FileIdByteArray.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(MessType, DataByteArray, Offset);
	WritePayload(RoomIdByteArray, DataByteArray, Offset);
	WritePayload(UserIdByteArray, DataByteArray, Offset);
	WritePayload(FileIdByteArray, DataByteArray, Offset);
}


void UDownloadFileRequest::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageType;
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> UserIdByteArray;
	TArray<uint8> FileIdByteArray;

	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	ReadPayload(ByteArray, RoomIdByteArray, Offset);
	ReadPayload(ByteArray, UserIdByteArray, Offset);
	ReadPayload(ByteArray, FileIdByteArray, Offset);

	PayloadData.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PayloadData.UserId = UNetworkUtils::ByteArrayToString(UserIdByteArray);
	PayloadData.FileId = UNetworkUtils::ByteArrayToString(FileIdByteArray);
}
