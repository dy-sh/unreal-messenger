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
	MessType = (int32) EClientServerMessageType::DownloadFileResponse;
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
		1 + //MessageType
		1 + //bSuccess
		BYTE_ARRAY_SIZE_BIT_DEPTH + RoomIdByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + UserIdByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + FileNameByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + FileIdByteArray.Num() +
		BYTE_ARRAY_SIZE_BIT_DEPTH + FileInfo.FileContent.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(MessType, DataByteArray, Offset);
	WritePayload(FileInfo.bSuccess, DataByteArray, Offset);
	WritePayload(RoomIdByteArray, DataByteArray, Offset);
	WritePayload(UserIdByteArray, DataByteArray, Offset);
	WritePayload(FileNameByteArray, DataByteArray, Offset);
	WritePayload(FileIdByteArray, DataByteArray, Offset);
	WritePayload(FileInfo.FileContent, DataByteArray, Offset);
}


void UDownloadFileResponse::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageType;
	TArray<uint8> RoomIdByteArray;
	TArray<uint8> UserIdByteArray;
	TArray<uint8> FileNameByteArray;
	TArray<uint8> FileIdByteArray;

	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	ReadPayload(ByteArray, PayloadData.bSuccess, Offset);
	ReadPayload(ByteArray, RoomIdByteArray, Offset);
	ReadPayload(ByteArray, UserIdByteArray, Offset);
	ReadPayload(ByteArray, FileNameByteArray, Offset);
	ReadPayload(ByteArray, FileIdByteArray, Offset);
	ReadPayload(ByteArray, PayloadData.FileContent, Offset);

	PayloadData.RoomId = UNetworkUtils::ByteArrayToString(RoomIdByteArray);
	PayloadData.UserId = UNetworkUtils::ByteArrayToString(UserIdByteArray);
	PayloadData.FileName = UNetworkUtils::ByteArrayToString(FileNameByteArray);
	PayloadData.FileId = UNetworkUtils::ByteArrayToString(FileIdByteArray);
}
