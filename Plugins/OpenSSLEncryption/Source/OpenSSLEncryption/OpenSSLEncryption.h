// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once

#include "Modules/ModuleManager.h"

class FOpenSSLEncryptionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
