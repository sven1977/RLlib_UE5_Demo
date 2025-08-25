// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class RLlib_UE5_DemoServerTarget : TargetRules
{
    public RLlib_UE5_DemoServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
	ExtraModuleNames.AddRange( new string[] { "RLlib_UE5_Demo" } );
    }
}
