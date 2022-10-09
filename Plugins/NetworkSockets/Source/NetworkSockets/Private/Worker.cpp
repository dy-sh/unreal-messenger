// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#include "Worker.h"
#include "Runtime/Core/Public/HAL/PlatformProcess.h"


FWorker::FWorker(const TFunction<EWorkerResult()> WorkDelegate, const float RepeatInterval)
	: bIsWorking(true),
	  CheckInterval(RepeatInterval),
	  OnWorkDelegate(WorkDelegate),
	  OnEndDelegate([]()
	  {
	  })
{
}


FWorker::FWorker(const TFunction<EWorkerResult()> WorkDelegate, const TFunction<void()> EndDelegate,
                 const float RepeatInterval)
	: bIsWorking(true),
	  CheckInterval(RepeatInterval),
	  OnWorkDelegate(WorkDelegate),
	  OnEndDelegate(EndDelegate)
{
}


uint32 FWorker::Run()
{
	while (bIsWorking)
	{
		if (OnWorkDelegate() == EWorkerResult::Completed) { return 0; }

		if (bIsWorking)
		{
			FPlatformProcess::Sleep(CheckInterval);
		}
	}

	return 0;
}


void FWorker::Stop()
{
	bIsWorking = false;
}


void FWorker::Exit()
{
	bIsWorking = false;
	OnEndDelegate();
}
