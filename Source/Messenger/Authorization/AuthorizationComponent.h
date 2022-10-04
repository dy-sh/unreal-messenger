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
	void ServerSetClientPublicKey(const FString& ClientPublicKey);

	UFUNCTION(Client, Reliable)
	void ClientSetServerPublicKey(const FString& ServerPublicKey);

	UFUNCTION(Server, Reliable)
	void ServerSetClientAesKey(const FString& EncryptedClientAesKey, const FString& EncryptedClientAesIvec);

	UFUNCTION(Client, Reliable)
	void ClientSetServerAesKey(const FString& EncryptedServerAesKey, const FString& EncryptedServerAesIvec);

	/* A client without a private key will not be able to decrypt the server's AES key and send an encrypted message. */
	UFUNCTION(Server, Reliable)
	void ServerAuthorizeClient(const FString& EncryptedText, const int32 PayloadSize);

	UFUNCTION(Client, Reliable)
	void ClientAuthorizationComplete(const EAuthorizationState State);


	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsKeysExchangeCompleted() const { return KeysExchangeCompleted; }


	UFUNCTION(BlueprintCallable)
	FORCEINLINE EAuthorizationState GetClientAuthorizationState() const { return ClientAuthorizationState; }


	UFUNCTION(BlueprintCallable)
	FString GetClientPublicKey() const { return ClientEncryptionKeys.PublicKey; }


	UFUNCTION(BlueprintCallable)
	FEncryptionKeys GetClientEncryptionKeys() const { return ClientEncryptionKeys; }


	UFUNCTION(BlueprintCallable)
	FEncryptionKeys GetServerEncryptionKeys() const { return ServerEncryptionKeys; }


private:
	FEncryptionKeys ClientEncryptionKeys;
	FEncryptionKeys ServerEncryptionKeys;
	bool KeysExchangeCompleted;
	EAuthorizationState ClientAuthorizationState;
};
