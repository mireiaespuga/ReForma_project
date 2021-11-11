// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ReForma_projectEditorTarget : TargetRules
{
	public ReForma_projectEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "ReForma_project" } );
		ExtraModuleNames.AddRange(new string[] { "ReForma_projectEditor" });
	}
}
