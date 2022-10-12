// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "DownloadFileResponse.h"
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
	MessType = (uint8) EClientServerMessageType::DownloadFileResponse;
	PayloadData = FileInfo;

	const int32 Length =
		CalculatePayloadSize(MessType) +
		CalculatePayloadSize(FileInfo.bSuccess) +
		CalculatePayloadSize(FileInfo.RoomId) +
		CalculatePayloadSize(FileInfo.UserId) +
		CalculatePayloadSize(FileInfo.FileName) +
		CalculatePayloadSize(FileInfo.FileId) +
		CalculatePayloadSize(FileInfo.FileContent);

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(MessType, DataByteArray, Offset);
	WritePayload(FileInfo.bSuccess, DataByteArray, Offset);
	WritePayload(FileInfo.RoomId, DataByteArray, Offset);
	WritePayload(FileInfo.UserId, DataByteArray, Offset);
	WritePayload(FileInfo.FileName, DataByteArray, Offset);
	WritePayload(FileInfo.FileId, DataByteArray, Offset);
	WritePayload(FileInfo.FileContent, DataByteArray, Offset);
}


void UDownloadFileResponse::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	uint8 MessageType;

	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	checkf(MessageType==MessType, TEXT("Failed to deserialize message. Wrong message type!"));
	ReadPayload(ByteArray, PayloadData.bSuccess, Offset);
	ReadPayload(ByteArray, PayloadData.RoomId, Offset);
	ReadPayload(ByteArray, PayloadData.UserId, Offset);
	ReadPayload(ByteArray, PayloadData.FileName, Offset);
	ReadPayload(ByteArray, PayloadData.FileId, Offset);
	ReadPayload(ByteArray, PayloadData.FileContent, Offset);
}
