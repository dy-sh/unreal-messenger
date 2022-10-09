// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "DataPackage.h"




bool FDataPackage::GetPayloadSize(int32& Size)
{
	if (GetByteArraySize() < HEADER_SIZE) return false;

	const uint8* ReceivedData = GetByteArrayPtr();

	Size = 0;
	Size |= ReceivedData[0] << 24;
	Size |= ReceivedData[1] << 16;
	Size |= ReceivedData[2] << 8;
	Size |= ReceivedData[3];
	return true;
}




bool FDataPackage::HasReceivedPayload()
{
	int32 PayloadSize = 0;

	if (!GetPayloadSize(PayloadSize)) return false;
	if (GetByteArraySize() < HEADER_SIZE + PayloadSize) return false;
	return true;
}

bool FDataPackage::DequeueReceivedPayload(TArray<uint8>& ByteArray)
{
	int32 PayloadSize = 0;

	if (!GetPayloadSize(PayloadSize)) return false;
	if (GetByteArraySize() < HEADER_SIZE + PayloadSize) return false;

	ByteArray = GetPartOfByteArray(HEADER_SIZE, PayloadSize);

	RemoveFromStart(HEADER_SIZE + PayloadSize);

	return true;
}


TArray<uint8> FDataPackage::PayloadToDataPackage(const TArray<uint8>& ByteArray)
{
	TArray<uint8> PackageByteArray;
	PackageByteArray.SetNum(ByteArray.Num() + HEADER_SIZE, false);

	// write data length to header (4 bytes for int32)
	PackageByteArray[0] = ByteArray.Num() >> 24;
	PackageByteArray[1] = ByteArray.Num() >> 16;
	PackageByteArray[2] = ByteArray.Num() >> 8;
	PackageByteArray[3] = ByteArray.Num();

	FMemory::Memcpy(PackageByteArray.GetData() + HEADER_SIZE, ByteArray.GetData(), ByteArray.Num());

	return PackageByteArray;
}
