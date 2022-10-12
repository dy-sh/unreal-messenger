// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "UploadFileResponse.h"
#include "Messenger/Chat/ChatTypes.h"


UUploadFileResponse* UUploadFileResponse::CreateUploadFileResponse(const FUploadFileResponsePayload& Payload)
{
	auto* Obj = NewObject<UUploadFileResponse>();
	Obj->InitializeByPayload(Payload);
	return Obj;
}


const FUploadFileResponsePayload& UUploadFileResponse::ParseUploadFileResponsePayload(const TArray<uint8>& ByteArray)
{
	auto* Obj = NewObject<UUploadFileResponse>();
	Obj->InitializeByByteArray(ByteArray);
	return Obj->GetPayload();
}


void UUploadFileResponse::InitializeByPayload(const FUploadFileResponsePayload& FileInfo)
{
	PayloadData = FileInfo;

	const int32 Length =
		CalculatePayloadSize(GetMessageType()) +
		CalculatePayloadSize(FileInfo.bSuccess) +
		CalculatePayloadSize(FileInfo.FileId);

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(GetMessageType(), DataByteArray, Offset);
	WritePayload(FileInfo.bSuccess, DataByteArray, Offset);
	WritePayload(FileInfo.FileId, DataByteArray, Offset);
}


void UUploadFileResponse::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	uint8 MessageType;
	
	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	checkf(MessageType==GetMessageType(), TEXT("Failed to deserialize message. Wrong message type!"));
	ReadPayload(ByteArray, PayloadData.bSuccess, Offset);
	ReadPayload(ByteArray, PayloadData.FileId, Offset);
}
