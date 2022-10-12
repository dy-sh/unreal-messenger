// // Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "UploadFileRequest.h"
#include "Messenger/Chat/ChatTypes.h"


UUploadFileRequest* UUploadFileRequest::CreateUploadFileRequest(const FUploadFileRequestPayload& Payload)
{
	auto* Obj = NewObject<UUploadFileRequest>();
	Obj->InitializeByPayload(Payload);
	return Obj;
}


const FUploadFileRequestPayload& UUploadFileRequest::ParseUploadFileRequestPayload(const TArray<uint8>& ByteArray)
{
	auto* Obj = NewObject<UUploadFileRequest>();
	Obj->InitializeByByteArray(ByteArray);
	return Obj->GetPayload();
}


void UUploadFileRequest::InitializeByPayload(const FUploadFileRequestPayload& FileInfo)
{
	PayloadData = FileInfo;

	//todo: dont send user name

	const int32 Length =
		CalculatePayloadSize(GetMessageType())+
		CalculatePayloadSize(FileInfo.RoomId) +
		CalculatePayloadSize(FileInfo.FileId) +
		CalculatePayloadSize(FileInfo.UserId) +
		CalculatePayloadSize(FileInfo.UserName) +
		CalculatePayloadSize(FileInfo.FileName) +
		CalculatePayloadSize(FileInfo.FileContent);

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(GetMessageType(), DataByteArray, Offset);
	WritePayload(FileInfo.RoomId, DataByteArray, Offset);
	WritePayload(FileInfo.FileId, DataByteArray, Offset);
	WritePayload(FileInfo.UserId, DataByteArray, Offset);
	WritePayload(FileInfo.UserName, DataByteArray, Offset);
	WritePayload(FileInfo.FileName, DataByteArray, Offset);
	WritePayload(FileInfo.FileContent, DataByteArray, Offset);
}


void UUploadFileRequest::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	uint8 MessageType;	
	
	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	checkf(MessageType==GetMessageType(), TEXT("Failed to deserialize message. Wrong message type!"));
	ReadPayload(ByteArray, PayloadData.RoomId, Offset);
	ReadPayload(ByteArray, PayloadData.FileId, Offset);
	ReadPayload(ByteArray, PayloadData.UserId, Offset);
	ReadPayload(ByteArray, PayloadData.UserName, Offset);
	ReadPayload(ByteArray, PayloadData.FileName, Offset);
	ReadPayload(ByteArray, PayloadData.FileContent, Offset);
}
