// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "ConnectionBase.h"
#include "Worker.h"
#include "ConnectionTcp.generated.h"

UCLASS(Blueprintable)
class NETWORKSOCKETS_API UConnectionTcp : public UConnectionBase
{
	GENERATED_BODY()

public:
	void StartPolling(FSocket* ConnectionSocket);

	virtual void Close() override;

protected:
	FCriticalSection Lock;
	bool bIsClosingSocket;

	EWorkerResult ReceivedData();
};
