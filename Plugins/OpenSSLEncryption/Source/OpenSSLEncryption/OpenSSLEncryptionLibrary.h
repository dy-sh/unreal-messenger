// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once


#include "Kismet/BlueprintFunctionLibrary.h"
#include "OpenSSLEncryptionLibrary.generated.h"

typedef struct rsa_st RSA;

UCLASS()
class OPENSSLENCRYPTION_API UOpenSSLEncryptionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static void Test(const FString& Payload);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static bool GenerateRsaKeyFiles(const FString& Path);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static bool ReadRsaKeysFromFile(const FString& Path, FString& OutPublicKey, FString& OutPrivateKey);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static FString GetRandomString(int32 Length);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static FString SHA1(const FString& Data);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static bool EncryptRsaPublic(const FString& Payload, const FString& PublicKey, FString& EncryptedData);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static bool DecryptRsaPrivate(const FString& EncryptedData, const FString& PrivateKey, FString& Payload);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static FString GenerateAesKey();

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static FString GenerateAesIvec();

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static bool EncryptAes(const FString& Payload, const FString& Key, const FString& Ivec, FString& EncryptedData,
		int32& PaddedPayloadSize);

	UFUNCTION(BlueprintCallable, Category = "OpenSSLEncryption")
	static bool DecryptAes(const FString& EncryptedData, const int32& PaddedPayloadSize, const FString& Key, const FString& Ivec,
		FString& Payload);


private:
	static RSA* CreateRSA(const unsigned char* Key, const bool Public);
	static void FStringToCharArray(const FString& String, unsigned char* CharArray);
	static int EncryptRsaPublic(const unsigned char* Data, const int DataLength, const unsigned char* Key,
		unsigned char* Encrypted);
	static int EncryptPrivate(const unsigned char* Data, const int DataLength, const unsigned char* Key, unsigned char* Encrypted);
	static int DecryptPublic(const unsigned char* Data, const int DataLength, const unsigned char* Key, unsigned char* Decrypted);
	static int DecryptRsaPrivate(const unsigned char* Data, const int DataLength, const unsigned char* Key,
		unsigned char* Decrypted);
};
