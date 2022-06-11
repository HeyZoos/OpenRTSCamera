// Copyright 2022 Jesus Bracho All Rights Reserved.

using UnrealBuildTool;

public class OpenRTSCameraRuntime : ModuleRules
{
	public OpenRTSCameraRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
			}
		);

		PrivateIncludePaths.AddRange(
			new string[]
			{
			}
		);

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"EnhancedInput",
				"Slate",
				"SlateCore",
				"UMG"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
