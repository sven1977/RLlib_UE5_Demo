// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class RLlib_UE5_Demo : ModuleRules
{
	public RLlib_UE5_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
        // This is needed for compiling with pybind11 headers.
        bUseRTTI = true;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        // Include pybind11 header files:
        // ..............................
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/pybind11/include"));

        // Point to (anaconda) python library:
        // ........................
        // On Windows:
        PublicAdditionalLibraries.Add(@"C:\Programs\Anaconda3\envs\simulator\libs\python311.lib");
        // On Mac:
        //PublicAdditionalLibraries.Add(@"/Users/sven/anaconda3/envs/simulator/lib/libpython3.11.dylib");

        // Python (anaconda) include files:
        // ..............................
        // On Win:
        PublicIncludePaths.Add(@"C:\Programs\Anaconda3\envs\simulator\Include");
        // On Mac:
        //PublicIncludePaths.Add(@"/Users/sven/anaconda3/envs/simulator/include");
    }
}
