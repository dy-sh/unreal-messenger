// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


enum class EWorkerResult:uint8
{
	InProgress,
	Completed
};

class NETWORKSOCKETS_API FWorker : public FRunnable
{
public:
	FWorker(TFunction<EWorkerResult()> WorkDelegate, float RepeatInterval = 0.01f);
	FWorker(TFunction<EWorkerResult()> WorkDelegate, TFunction<void()> EndDelegate, float RepeatInterval = 0.01f);

	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	volatile bool bIsWorking;
	float CheckInterval;
	TFunction<EWorkerResult()> OnWorkDelegate;
	TFunction<void()> OnEndDelegate;
};
