// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

using UnrealBuildTool;

public class MessengerServerTarget : TargetRules
{
	public MessengerServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new[] { "Messenger" });
	}
}