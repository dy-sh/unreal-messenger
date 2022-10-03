// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

using UnrealBuildTool;

public class MessengerTarget : TargetRules
{
	public MessengerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new[] { "Messenger" });
	}
}