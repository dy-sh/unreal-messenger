// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "UploadFileResponse.h"
#include "NetworkUtils.h"
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
	MessType = (int32) EClientServerMessageType::UploadFileResponse;
	PayloadData = FileInfo;

	TArray<uint8> FileIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileId, FileIdByteArray);

	const int32 Length =
		1 + //MessageType
		1 + //bSuccess
		BYTE_ARRAY_SIZE_BIT_DEPTH + FileIdByteArray.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayload(MessType, DataByteArray, Offset);
	WritePayload(FileInfo.bSuccess, DataByteArray, Offset);
	WritePayload(FileIdByteArray, DataByteArray, Offset);
}


void UUploadFileResponse::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageType;
	TArray<uint8> FileIdByteArray;

	int32 Offset = 0;
	ReadPayload(ByteArray, MessageType, Offset);
	ReadPayload(ByteArray, PayloadData.bSuccess, Offset);
	ReadPayload(ByteArray, FileIdByteArray, Offset);

	PayloadData.FileId = UNetworkUtils::ByteArrayToString(FileIdByteArray);
}
