// // Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "UploadFileRequest.h"
#include "NetworkUtils.h"
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
	MessType = (int32) EClientServerMessageType::UploadFileRequest;
	PayloadData = FileInfo;

	TArray<uint8> RoomIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.RoomId, RoomIdByteArray);

	TArray<uint8> UserIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.UserId, UserIdByteArray);

	TArray<uint8> UserNameByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.UserName, UserNameByteArray); 

	TArray<uint8> FileNameByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileName, FileNameByteArray);

	const int32 Length =
		1 + //MessageType
		BYTE_ARRAY_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + UserIdByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + UserNameByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + FileNameByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + FileInfo.FileContent.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(MessType, DataByteArray, Offset);
	WritePayload(RoomIdByteArray, DataByteArray, Offset);
	WritePayload(UserIdByteArray, DataByteArray, Offset);
	WritePayload(UserNameByteArray, DataByteArray, Offset);
	WritePayload(FileNameByteArray, DataByteArray, Offset);
	WritePayload(FileInfo.FileContent, DataByteArray, Offset);
}


void UUploadFileRequest::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageType;
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> UserIdByteArray;
	TArray<uint8> UserNameByteArray;
	TArray<uint8> FileNameByteArray;

	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	ReadPayload(ByteArray, RoomIdByteArray, Offset);
	ReadPayload(ByteArray, UserIdByteArray, Offset);
	ReadPayload(ByteArray, UserNameByteArray, Offset);
	ReadPayload(ByteArray, FileNameByteArray, Offset);
	ReadPayload(ByteArray, PayloadData.FileContent, Offset);

	PayloadData.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PayloadData.UserId = UNetworkUtils::ByteArrayToString(UserIdByteArray);
	PayloadData.UserName = UNetworkUtils::ByteArrayToString(UserNameByteArray);
	PayloadData.FileName = UNetworkUtils::ByteArrayToString(FileNameByteArray);
}
