// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "OpenSSLEncryptionLibrary.h"


#include "OpenSSLEncryption.h"
#include "Serialization/BufferArchive.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
THIRD_PARTY_INCLUDES_END
#undef UI

DEFINE_LOG_CATEGORY_STATIC(LogOpenSSLEncryption, All, All);

constexpr int RSA_PADDING = RSA_PKCS1_PADDING;
constexpr int RSA_KEY_BITS = 2048;
constexpr int RSA_PUB_KEY_BYTES = 460;
constexpr int RSA_PRIV_KEY_BYTES = 1702;
constexpr int RSA_MAX_PAYLOAD_SIZE = 244;
constexpr unsigned long BN = RSA_F4;
constexpr int AES_KEY_BYTES = 32;
constexpr int AES_AVEC_BYTES = 16;
#define PUB_KEY_FILENAME "public.pem"
#define PRIV_KEY_FILENAME "private.pem"


RSA* UOpenSSLEncryptionLibrary::CreateRSA(const unsigned char* Key, const bool Public)
{
	BIO* Bio = BIO_new_mem_buf(Key, -1);
	if (!Bio)
	{
		UE_LOG(LogOpenSSLEncryption, Error, TEXT("Failed to create key BIO"));
		return nullptr;
	}

	RSA* Rsa = nullptr;
	if (Public)
	{
		Rsa = PEM_read_bio_RSA_PUBKEY(Bio, &Rsa, nullptr, nullptr);
	}
	else
	{
		Rsa = PEM_read_bio_RSAPrivateKey(Bio, &Rsa, nullptr, nullptr);
	}

	if (!Rsa)
	{
		UE_LOG(LogOpenSSLEncryption, Error, TEXT("Failed to create RSA"));
	}

	return Rsa;
}


void UOpenSSLEncryptionLibrary::FStringToCharArray(const FString& String, unsigned char* CharArray)
{
	for (int32 Index = 0; Index != String.Len(); ++Index)
	{
		*(CharArray + Index) = String[Index];
	}
}


bool UOpenSSLEncryptionLibrary::EncryptRsaPublic(const FString& Payload, const FString& PublicKey, FString& EncryptedData)
{
	unsigned char KeyArr[RSA_PUB_KEY_BYTES] = {};
	FStringToCharArray(PublicKey, KeyArr);

	// serialize FString to byte array
	const FTCHARToUTF8 ToUtf8Converter(Payload.GetCharArray().GetData());
	const int32 PayloadSize = ToUtf8Converter.Length();
	const ANSICHAR* UtfPayload = ToUtf8Converter.Get();

	if (PayloadSize > RSA_MAX_PAYLOAD_SIZE)
	{
		UE_LOG(LogOpenSSLEncryption, Error, TEXT("Failed to encrypt with public key. Payload size is too large!"));
		return false;
	}

	unsigned char EncryptedArr[RSA_PUB_KEY_BYTES] = {};

	const int EncryptedSize = EncryptRsaPublic((unsigned char*) UtfPayload, PayloadSize, KeyArr, EncryptedArr);
	if (EncryptedSize == -1)
	{
		UE_LOG(LogOpenSSLEncryption, Error, TEXT("Failed to encrypt with public key"));
		return false;
	}

	EncryptedData = FBase64::Encode(EncryptedArr, EncryptedSize);
	return true;
}


int UOpenSSLEncryptionLibrary::EncryptRsaPublic(const unsigned char* Data, const int DataLength, const unsigned char* Key,
	unsigned char* Encrypted)
{
	RSA* Rsa = CreateRSA(Key, true);
	const int Result = RSA_public_encrypt(DataLength, Data, Encrypted, Rsa, RSA_PADDING);
	return Result;
}


bool UOpenSSLEncryptionLibrary::DecryptRsaPrivate(const FString& EncryptedData, const FString& PrivateKey, FString& Payload)
{
	TArray<unsigned char> EncryptedArr;
	FBase64::Decode(EncryptedData, EncryptedArr);

	unsigned char KeyArr[RSA_PRIV_KEY_BYTES] = {};
	FStringToCharArray(PrivateKey, KeyArr);

	unsigned char DecryptedArr[RSA_PUB_KEY_BYTES] = {};

	int32 PayloadSize = EncryptedArr.Num();
	const int Result = DecryptRsaPrivate(EncryptedArr.GetData(), PayloadSize, KeyArr, DecryptedArr);
	if (Result == -1)
	{
		UE_LOG(LogOpenSSLEncryption, Error, TEXT("Failed to decrypt with private key"));
		return false;
	}

	// deserialize byte array to FString
	FUTF8ToTCHAR ToTCharConverter((ANSICHAR*) DecryptedArr);
	Payload = ToTCharConverter.Get();

	return true;
}


FString UOpenSSLEncryptionLibrary::GenerateAesKey()
{
	return GetRandomString(AES_KEY_BYTES);
}


FString UOpenSSLEncryptionLibrary::GenerateAesIvec()
{
	return GetRandomString(AES_AVEC_BYTES);
}


int UOpenSSLEncryptionLibrary::DecryptRsaPrivate(const unsigned char* Data, const int DataLength, const unsigned char* Key,
	unsigned char* Decrypted)
{
	RSA* Rsa = CreateRSA(Key, false);
	const int Result = RSA_private_decrypt(DataLength, Data, Decrypted, Rsa, RSA_PADDING);
	return Result;
}


int UOpenSSLEncryptionLibrary::EncryptPrivate(const unsigned char* Data, const int DataLength, const unsigned char* Key,
	unsigned char* Encrypted)
{
	RSA* Rsa = CreateRSA(Key, false);
	const int Result = RSA_private_encrypt(DataLength, Data, Encrypted, Rsa, RSA_PADDING);
	return Result;
}


int UOpenSSLEncryptionLibrary::DecryptPublic(const unsigned char* Data, const int DataLength, const unsigned char* Key,
	unsigned char* Decrypted)
{
	RSA* Rsa = CreateRSA(Key, true);
	const int Result = RSA_public_decrypt(DataLength, Data, Decrypted, Rsa, RSA_PADDING);
	return Result;
}


bool UOpenSSLEncryptionLibrary::EncryptAes(const FString& Payload, const FString& Key, const FString& Ivec,
	FString& EncryptedData, int32& PaddedPayloadSize)
{
	unsigned char KeyArr[AES_KEY_BYTES] = {};
	FStringToCharArray(Key, KeyArr);

	unsigned char IvecArr[AES_AVEC_BYTES] = {};
	FStringToCharArray(Ivec, IvecArr);

	// serialize FString to byte array
	const FTCHARToUTF8 ToUtf8Converter(Payload.GetCharArray().GetData());
	const int32 PayloadSize = ToUtf8Converter.Length();
	const ANSICHAR* UtfPayload = ToUtf8Converter.Get();

	AES_KEY AesKey;
	AES_set_encrypt_key(KeyArr, AES_KEY_BYTES * 8, &AesKey);

	const int Padding = AES_BLOCK_SIZE - (PayloadSize % AES_BLOCK_SIZE);
	TArray<unsigned char> PaddedPayload;
	PaddedPayload.Append((unsigned char*) UtfPayload, PayloadSize);
	for (int i = 0; i < Padding; i++)
	{
		PaddedPayload.Add(0);
	}

	unsigned char* PayloadArr = &PaddedPayload[0];
	PaddedPayloadSize = (const int) PaddedPayload.Num();

	unsigned char* EncryptedArr = new unsigned char[PaddedPayloadSize];

	AES_cbc_encrypt(PayloadArr, EncryptedArr, PaddedPayloadSize, &AesKey, IvecArr, AES_ENCRYPT);

	EncryptedData = FBase64::Encode(EncryptedArr, PaddedPayloadSize);
	delete[] EncryptedArr;
	return true;
}


bool UOpenSSLEncryptionLibrary::DecryptAes(const FString& EncryptedData, const int32& PaddedPayloadSize, const FString& Key,
	const FString& Ivec, FString& Payload)
{
	TArray<unsigned char> EncryptedArr;
	FBase64::Decode(EncryptedData, EncryptedArr);

	unsigned char KeyArr[AES_KEY_BYTES] = {};
	FStringToCharArray(Key, KeyArr);

	unsigned char IvecArr[AES_AVEC_BYTES] = {};
	FStringToCharArray(Ivec, IvecArr);

	unsigned char* DecryptedArr = new unsigned char[PaddedPayloadSize];

	AES_KEY AesKey;

	AES_set_decrypt_key(KeyArr, AES_KEY_BYTES * 8, &AesKey);
	AES_cbc_encrypt(EncryptedArr.GetData(), DecryptedArr, PaddedPayloadSize, &AesKey, IvecArr, AES_DECRYPT);

	// deserialize byte array to FString
	FUTF8ToTCHAR ToTCharConverter((ANSICHAR*) DecryptedArr);

	Payload = ToTCharConverter.Get();
	delete[] DecryptedArr;

	return true;
}


bool UOpenSSLEncryptionLibrary::GenerateRsaKeyFiles(const FString& Path)
{
	RSA* Rsa = nullptr;
	BIO* BioPublic = nullptr,* BioPrivate = nullptr;

	BIGNUM* Bne = BN_new();
	int Success = BN_set_word(Bne, BN);
	if (Success == 1)
	{
		Rsa = RSA_new();
		Success = RSA_generate_key_ex(Rsa, RSA_KEY_BITS, Bne, nullptr);
		if (Success == 1)
		{
			const char* Filename = (char*) TCHAR_TO_ANSI(*(Path + PUB_KEY_FILENAME));
			BioPublic = BIO_new_file(Filename, "w+");
			Success = PEM_write_bio_RSA_PUBKEY(BioPublic, Rsa);
			if (Success == 1)
			{
				Filename = (char*) TCHAR_TO_ANSI(*(Path + PRIV_KEY_FILENAME));
				BioPrivate = BIO_new_file(Filename, "w+");
				Success = PEM_write_bio_RSAPrivateKey(BioPrivate, Rsa, nullptr, nullptr, 0, nullptr, nullptr);
			}
		}
	}

	BIO_free_all(BioPublic);
	BIO_free_all(BioPrivate);
	RSA_free(Rsa);
	BN_free(Bne);

	return Success == 1;
}


bool UOpenSSLEncryptionLibrary::ReadRsaKeysFromFile(const FString& Path, FString& OutPublicKey, FString& OutPrivateKey)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString Filename = Path + PUB_KEY_FILENAME;
	if (!FileManager.FileExists(*Filename))
	{
		UE_LOG(LogOpenSSLEncryption, Warning, TEXT("File not found: %s"), *Filename);
		return false;
	}
	if (!FFileHelper::LoadFileToString(OutPublicKey, *Filename, FFileHelper::EHashOptions::None))
	{
		UE_LOG(LogOpenSSLEncryption, Warning, TEXT("Failed to read file %s"), *Filename);
		return false;
	}

	Filename = Path + PRIV_KEY_FILENAME;
	if (!FileManager.FileExists(*Filename))
	{
		UE_LOG(LogOpenSSLEncryption, Warning, TEXT("File not found: %s"), *Filename);
		return false;
	}
	if (!FFileHelper::LoadFileToString(OutPrivateKey, *Filename, FFileHelper::EHashOptions::None))
	{
		UE_LOG(LogOpenSSLEncryption, Warning, TEXT("Failed to read file %s"), *Filename);
		return false;
	}

	return true;
}


FString UOpenSSLEncryptionLibrary::GetRandomString(int32 Length)
{
	FString ValidChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

	FString Result;
	Result.Empty(Length);

	for (int i = 0; i < Length; ++i)
	{
		int32 Index = FMath::RandRange(0, ValidChars.Len() - 1);
		Result.AppendChar(ValidChars[Index]);
	}
	return Result;
}


FString UOpenSSLEncryptionLibrary::SHA1(const FString& Data)
{
	const std::string Str(TCHAR_TO_UTF8(*Data));

	FSHA1 HashState;
	HashState.Update((uint8*) Str.c_str(), Str.size());
	HashState.Final();

	uint8 Hash[FSHA1::DigestSize];
	HashState.GetHash(Hash);
	return BytesToHex(Hash, FSHA1::DigestSize);
}


void UOpenSSLEncryptionLibrary::Test(const FString& Payload)
{
	const FString SavedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());

	FString PublicKey;
	FString PrivateKey;

	if (!ReadRsaKeysFromFile(SavedPath, PublicKey, PrivateKey))
	{
		GenerateRsaKeyFiles(SavedPath);
		ReadRsaKeysFromFile(SavedPath, PublicKey, PrivateKey);
	}

	if (PublicKey.IsEmpty() || PrivateKey.IsEmpty())
	{
		UE_LOG(LogOpenSSLEncryption, Error, TEXT("Failed to read or generate keys"))
		return;
	}

	FString EncryptedData, DecryptedPayload;
	if (EncryptRsaPublic(Payload, PublicKey, EncryptedData))
	{
		UE_LOG(LogOpenSSLEncryption, Warning, TEXT("Encrypted length: %d"), EncryptedData.Len());

		if (DecryptRsaPrivate(EncryptedData, PrivateKey, DecryptedPayload))
		{
			UE_LOG(LogOpenSSLEncryption, Warning, TEXT("Decrypted: %s"), *DecryptedPayload);
		}
	}

	const FString AesKey = GenerateAesKey();
	const FString AesIvec = GenerateAesIvec();

	int32 PayloadSize;
	EncryptAes(Payload, AesKey, AesIvec, EncryptedData, PayloadSize);
	DecryptAes(EncryptedData, PayloadSize, AesKey, AesIvec, DecryptedPayload);

	UE_LOG(LogOpenSSLEncryption, Warning, TEXT("Decrypted AES: %s"), *DecryptedPayload);
}
