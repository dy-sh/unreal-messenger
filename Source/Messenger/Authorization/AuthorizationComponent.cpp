// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "AuthorizationComponent.h"
#include "OpenSSLEncryption/OpenSSLEncryptionBPLibrary.h"

class UChatServerComponent;

DEFINE_LOG_CATEGORY_STATIC(LogAuthorizationComponent, All, All);


UAuthorizationComponent::UAuthorizationComponent()
{
	SetIsReplicatedByDefault(true);
}




void UAuthorizationComponent::BeginPlay()
{
	Super::BeginPlay();

	GenerateEncryptionKeys();

	if (auto* World = GetWorld())
	{
		if (World->GetNetMode() != ENetMode::NM_DedicatedServer)
		{
			ExchangeKeysWithServer();
		}
	}
}


void UAuthorizationComponent::GenerateEncryptionKeys()
{
	const FString SavedPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());

	FString PublicKey;
	FString PrivateKey;

	if (!UOpenSSLEncryptionBPLibrary::ReadRsaKeysFromFile(SavedPath, PublicKey, PrivateKey))
	{
		UOpenSSLEncryptionBPLibrary::GenerateRsaKeyFiles(SavedPath);
		UOpenSSLEncryptionBPLibrary::ReadRsaKeysFromFile(SavedPath, PublicKey, PrivateKey);
	}

	if (PublicKey.IsEmpty() || PrivateKey.IsEmpty())
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Failed to read or generate encryption keys"))
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		ServerEncryptionKeys.PublicKey = PublicKey;
		ServerEncryptionKeys.PrivateKey = PrivateKey;
		ServerEncryptionKeys.AesKey = UOpenSSLEncryptionBPLibrary::GenerateAesKey();
		ServerEncryptionKeys.AesIvec = UOpenSSLEncryptionBPLibrary::GenerateAesIvec();
		UE_LOG(LogAuthorizationComponent, Display, TEXT("Server keys generated"))
	}

	if (GetWorld())
	{
		if (GetWorld()->GetNetMode() != ENetMode::NM_DedicatedServer)
		{
			ClientEncryptionKeys.PublicKey = PublicKey;
			ClientEncryptionKeys.PrivateKey = PrivateKey;
			ClientEncryptionKeys.AesKey = UOpenSSLEncryptionBPLibrary::GenerateAesKey();
			ClientEncryptionKeys.AesIvec = UOpenSSLEncryptionBPLibrary::GenerateAesIvec();
			UE_LOG(LogAuthorizationComponent, Display, TEXT("Client keys generated"))
		}
	}
}


void UAuthorizationComponent::ExchangeKeysWithServer()
{
	if (GetWorld())
	{
		if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
		{
			UE_LOG(LogAuthorizationComponent, Error, TEXT("Call ExchangeKeysWithServer function only on the client!"))
			return;
		}
	}

	if (ClientEncryptionKeys.PublicKey.IsEmpty())
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to exchange encryption keys with server. Keys are not generated."))
		return;
	}

	ServerReceivePublicKey(ClientEncryptionKeys.PublicKey);
}


void UAuthorizationComponent::ServerReceivePublicKey_Implementation(const FString& ClientPublicKey)
{
	UE_LOG(LogAuthorizationComponent, Display, TEXT("Client public key received"));

	ClientEncryptionKeys.PublicKey = ClientPublicKey;

	ClientReceivePublicKey(ServerEncryptionKeys.PublicKey);
}


void UAuthorizationComponent::ClientReceivePublicKey_Implementation(const FString& ServerPublicKey)
{
	UE_LOG(LogAuthorizationComponent, Display, TEXT("Server public key received"));

	ServerEncryptionKeys.PublicKey = ServerPublicKey;

	FString EncryptedClientAesKey;
	if (!UOpenSSLEncryptionBPLibrary::EncryptRsaPublic(ClientEncryptionKeys.AesKey, ServerPublicKey, EncryptedClientAesKey))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to encrypt client AES key"))
		return;
	}

	FString EncryptedClientAesIvec;
	if (!UOpenSSLEncryptionBPLibrary::EncryptRsaPublic(ClientEncryptionKeys.AesIvec, ServerPublicKey, EncryptedClientAesIvec))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to encrypt client AES ivec"))
		return;
	}

	ServerReceiveAesKey(EncryptedClientAesKey, EncryptedClientAesIvec);
}


void UAuthorizationComponent::ServerReceiveAesKey_Implementation(const FString& EncryptedClientAesKey,
	const FString& EncryptedClientAesIvec)
{
	UE_LOG(LogAuthorizationComponent, Display, TEXT("Client AES key received"));

	FString DecryptedClientAesKey;
	if (!UOpenSSLEncryptionBPLibrary::DecryptRsaPrivate(EncryptedClientAesKey, ServerEncryptionKeys.PrivateKey,
		DecryptedClientAesKey))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to decrypt client AES key"))
		return;
	}

	FString DecryptedClientAesIvec;
	if (!UOpenSSLEncryptionBPLibrary::DecryptRsaPrivate(EncryptedClientAesIvec, ServerEncryptionKeys.PrivateKey,
		DecryptedClientAesIvec))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to decrypt client AES ivec"))
		return;
	}

	if (DecryptedClientAesKey.IsEmpty() || DecryptedClientAesIvec.IsEmpty())
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to decrypt client AES key or ivec"))
		return;
	}

	ClientEncryptionKeys.AesKey = DecryptedClientAesKey;
	ClientEncryptionKeys.AesIvec = DecryptedClientAesIvec;

	FString EncryptedServerAesKey;
	if (!UOpenSSLEncryptionBPLibrary::EncryptRsaPublic(ServerEncryptionKeys.AesKey, ClientEncryptionKeys.PublicKey,
		EncryptedServerAesKey))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to encrypt server AES key"))
		return;
	}

	FString EncryptedServerAesIvec;
	if (!UOpenSSLEncryptionBPLibrary::EncryptRsaPublic(ServerEncryptionKeys.AesIvec, ClientEncryptionKeys.PublicKey,
		EncryptedServerAesIvec))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to encrypt server AES ivec"))
		return;
	}

	if (GetNetMode() == ENetMode::NM_DedicatedServer || GetOwner() != GetWorld()->GetFirstPlayerController())
	{
		KeysExchangeCompleted = true;
		OnEncryptionKeysExchangeComplete.Broadcast();
	}

	ClientReceiveAesKey(EncryptedServerAesKey, EncryptedServerAesIvec);
}


void UAuthorizationComponent::ClientReceiveAesKey_Implementation(const FString& EncryptedServerAesKey,
	const FString& EncryptedServerAesIvec)
{
	UE_LOG(LogAuthorizationComponent, Display, TEXT("Server AES key received"));

	FString DecryptedServerAesKey;
	if (!UOpenSSLEncryptionBPLibrary::DecryptRsaPrivate(EncryptedServerAesKey, ClientEncryptionKeys.PrivateKey,
		DecryptedServerAesKey))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to decrypt server AES key"))
		return;
	}

	FString DecryptedServerAesIvec;
	if (!UOpenSSLEncryptionBPLibrary::DecryptRsaPrivate(EncryptedServerAesIvec, ClientEncryptionKeys.PrivateKey,
		DecryptedServerAesIvec))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to decrypt server AES ivec"))
		return;
	}

	if (DecryptedServerAesKey.IsEmpty() || DecryptedServerAesIvec.IsEmpty())
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to decrypt server AES key or ivec"))
		return;
	}

	ServerEncryptionKeys.AesKey = DecryptedServerAesKey;
	ServerEncryptionKeys.AesIvec = DecryptedServerAesIvec;

	UE_LOG(LogAuthorizationComponent, Display, TEXT("Exchange keys complete"))

	KeysExchangeCompleted = true;
	OnEncryptionKeysExchangeComplete.Broadcast();

	// Authorize
	FString EncryptedMessage;
	FString EncryptedData;
	int32 PayloadSize;
	if (!UOpenSSLEncryptionBPLibrary::EncryptAes("AUTHORIZE", ServerEncryptionKeys.AesKey, ServerEncryptionKeys.AesIvec,
		EncryptedData, PayloadSize))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to encrypt client authorize message"))
		return;
	}

	ServerRequestAuthorization(EncryptedData, PayloadSize);
}


void UAuthorizationComponent::ServerRequestAuthorization_Implementation(const FString& EncryptedText, const int32 PayloadSize)
{
	UE_LOG(LogAuthorizationComponent, Display, TEXT("Server received client authorization request"));

	FString DecryptedText;
	if (!UOpenSSLEncryptionBPLibrary::DecryptAes(EncryptedText, PayloadSize, ServerEncryptionKeys.AesKey,
		ServerEncryptionKeys.AesIvec, DecryptedText))
	{
		UE_LOG(LogAuthorizationComponent, Error, TEXT("Faild to decrypt client authorization request"))
		return;
	}

	ClientAuthorizationState = DecryptedText == "AUTHORIZE" ? EAuthorizationState::Authorized : EAuthorizationState::Failed;

	if (GetNetMode() == ENetMode::NM_DedicatedServer || GetOwner() != GetWorld()->GetFirstPlayerController())
	{
		OnAuthorizationComplete.Broadcast(ClientAuthorizationState, ClientEncryptionKeys, ServerEncryptionKeys);
	}

	ClientRespondAuthorization(ClientAuthorizationState);
}


void UAuthorizationComponent::ClientRespondAuthorization_Implementation(const EAuthorizationState State)
{
	if (State == EAuthorizationState::Authorized)
	{
		UE_LOG(LogAuthorizationComponent, Display, TEXT("Client authirization complete"));
	}
	else
	{
		UE_LOG(LogAuthorizationComponent, Display, TEXT("Client authirization rejected"));
	}

	ClientAuthorizationState = State;
	OnAuthorizationComplete.Broadcast(ClientAuthorizationState, ClientEncryptionKeys, ServerEncryptionKeys);
}
