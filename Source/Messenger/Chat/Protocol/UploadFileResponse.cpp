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
	MessType = (int32)EClientServerMessageType::UploadFileResponse;
	PayloadData = FileInfo;

	TArray<uint8> FileIdByteArray;
	UNetworkUtils::StringToByteArray(FileInfo.FileId, FileIdByteArray);

	const int32 Length =
		DATA_SIZE_BIT_DEPTH + //MessageType
		1 +                   //bSuccess
		DATA_SIZE_BIT_DEPTH + FileIdByteArray.Num();

	DataByteArray.SetNum(Length, false);

	int32 Offset = 0;
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, MessType, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileInfo.bSuccess, DataByteArray, Offset);
	WritePayloadToDataByteArray(DATA_SIZE_BIT_DEPTH, FileIdByteArray, DataByteArray, Offset);
}


void UUploadFileResponse::InitializeByByteArray(const TArray<uint8>& ByteArray)
{
	int32 MessageType;
	TArray<uint8> FileIdByteArray;

	int32 Offset = 0;
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, MessageType, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, PayloadData.bSuccess, Offset);
	ReadPayloadFromDataByteArray(DATA_SIZE_BIT_DEPTH, ByteArray, FileIdByteArray, Offset);
	
	PayloadData.FileId = UNetworkUtils::ByteArrayToString(FileIdByteArray);
}
