// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "DownloadFileRequest.h"
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
	PayloadData = FileInfo;

	const int32 Length =
		CalculatePayloadSize(GetMessageType()) +
		CalculatePayloadSize(FileInfo.RoomId) +
		CalculatePayloadSize(FileInfo.UserId) +
		CalculatePayloadSize(FileInfo.FileId);

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(GetMessageType(), DataByteArray, Offset);
	WritePayload(FileInfo.RoomId, DataByteArray, Offset);
	WritePayload(FileInfo.UserId, DataByteArray, Offset);
	WritePayload(FileInfo.FileId, DataByteArray, Offset);
}


void UDownloadFileRequest::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	uint8 MessageType;

	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	checkf(MessageType==GetMessageType(), TEXT("Failed to deserialize message. Wrong message type!"));
	ReadPayload(ByteArray, PayloadData.RoomId, Offset);
	ReadPayload(ByteArray, PayloadData.UserId, Offset);
	ReadPayload(ByteArray, PayloadData.FileId, Offset);
}
