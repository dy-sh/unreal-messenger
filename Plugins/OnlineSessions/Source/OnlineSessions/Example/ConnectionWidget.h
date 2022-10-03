// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessions/Types.h"
#include "ConnectionWidget.generated.h"


UCLASS()
class ONLINESESSIONS_API UConnectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category=OnlineSessions)
	void Configure(const FSessionParams& Params);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* StartServerButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerIPTextBox;

	UPROPERTY(meta = (BindWidget))
	UButton* ConnectToIPButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FindServerButton;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FSessionParams SessionParams;

	class UOnlineSessionsSubsystem* OnlineSessionsSubsystem;

	
	virtual bool Initialize() override;

	UFUNCTION()
	void OnStartServerButtonClicked();

	UFUNCTION()
	void OnFindServerButtonClicked();

	UFUNCTION()
	void OnConnectToIPButtonClicked();

	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

	
	void EnableButtons(bool bEnable);
};
