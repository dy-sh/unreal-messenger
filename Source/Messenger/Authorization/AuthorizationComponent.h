// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "AuthorizationTypes.h"
#include "AuthorizationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEncryptionKeysExchangeComplete);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAuthorizationComplete, const EAuthorizationState, State, const FEncryptionKeys&,
	ClientEncryptionKeys, const FEncryptionKeys&, ServerEncryptionKeys);




UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MESSENGER_API UAuthorizationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnEncryptionKeysExchangeComplete OnEncryptionKeysExchangeComplete;

	UPROPERTY(BlueprintAssignable)
	FOnAuthorizationComplete OnAuthorizationComplete;

	UAuthorizationComponent();

	virtual void BeginPlay() override;

	UFUNCTION()
	void GenerateEncryptionKeys();

	UFUNCTION()
	void ExchangeKeysWithServer();

	UFUNCTION(Server, Reliable)
	void ServerReceivePublicKey(const FString& ClientPublicKey);

	UFUNCTION(Client, Reliable)
	void ClientReceivePublicKey(const FString& ServerPublicKey);

	UFUNCTION(Server, Reliable)
	void ServerReceiveAesKey(const FString& EncryptedClientAesKey, const FString& EncryptedClientAesIvec);

	UFUNCTION(Client, Reliable)
	void ClientReceiveAesKey(const FString& EncryptedServerAesKey, const FString& EncryptedServerAesIvec);

	/* A client without a private key will not be able to decrypt the server's AES key and send an encrypted message. */
	UFUNCTION(Server, Reliable)
	void ServerRequestAuthorization(const FString& EncryptedText, const int32 PayloadSize);

	UFUNCTION(Client, Reliable)
	void ClientRespondAuthorization(const EAuthorizationState State);


	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsKeysExchangeCompleted() { return KeysExchangeCompleted; }


	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAuthorizationState GetClientAuthorizationState() { return ClientAuthorizationState; }


	UFUNCTION(BlueprintCallable)
	const FString& GetClientPublicKey() { return ClientEncryptionKeys.PublicKey; }


	UFUNCTION(BlueprintCallable)
	const FEncryptionKeys& GetClientEncryptionKeys() { return ClientEncryptionKeys; }


	UFUNCTION(BlueprintCallable)
	const FEncryptionKeys& GetServerEncryptionKeys() { return ServerEncryptionKeys; }


private:
	FEncryptionKeys ClientEncryptionKeys;
	FEncryptionKeys ServerEncryptionKeys;
	bool KeysExchangeCompleted;
	EAuthorizationState ClientAuthorizationState;
};
