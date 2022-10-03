// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

using UnrealBuildTool;

public class MessengerClientTarget : TargetRules
{
	public MessengerClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new[] { "Messenger" });
	}
}