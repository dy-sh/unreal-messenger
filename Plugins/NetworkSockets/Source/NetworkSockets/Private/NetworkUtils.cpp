// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "NetworkUtils.h"
#include <string>


void UNetworkUtils::StringToByteArray(const FString& String, TArray<uint8>& ByteArray)
{
	std::string Str = TCHAR_TO_UTF8(*String);
  
	ByteArray.SetNum(Str.size() + 1);  
	ByteArray[ByteArray.Num() - 1] = 0x00;  
  
	FMemory::Memcpy(ByteArray.GetData(), Str.c_str(), Str.size());  
}


FString UNetworkUtils::ByteArrayToString(const TArray<uint8>& ByteArray)
{
	if (ByteArray[ByteArray.Num() - 1] == 0x00)
	{
		return UTF8_TO_TCHAR(ByteArray.GetData());
	}

	TArray<uint8> Arr;
	Arr.SetNum(ByteArray.Num() + 1);
	FMemory::Memcpy(Arr.GetData(), ByteArray.GetData(), ByteArray.Num());
	Arr[Arr.Num() - 1] = 0x00;

	return UTF8_TO_TCHAR(Arr.GetData());
}


bool UNetworkUtils::IpStringToByteArray(FString IpAddress, TArray<uint8>& ByteArray)
{
	if (IpAddress.ToLower() == TEXT("localhost"))
	{
		IpAddress = "127.0.0.1";
	}

	const TCHAR* DelimArray[] = {TEXT(".")};

	TArray<FString> Octets;
	IpAddress.ParseIntoArray(Octets, DelimArray, true);

	if (Octets.Num() != 4) return false;

	ByteArray.SetNum(4);
	for (int32 i = 0; i < 4; ++i)
	{
		ByteArray[i] = FCString::Atoi(*Octets[i]);
	}

	return true;
}


bool UNetworkUtils::GetEndpoint(const FString& IpAddress, const int32 Port, FIPv4Endpoint& OutEndpoint)
{
	TArray<uint8> Arr;
	if (!IpStringToByteArray(IpAddress, Arr))
	{
		return false;
	}

	OutEndpoint = FIPv4Endpoint(FIPv4Address(Arr[0], Arr[1], Arr[2], Arr[3]), Port);

	return true;
}
