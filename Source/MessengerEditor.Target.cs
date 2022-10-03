// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

using UnrealBuildTool;

public class MessengerEditorTarget : TargetRules
{
	public MessengerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new[] { "Messenger" });
	}
}