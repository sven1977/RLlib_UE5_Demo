// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RLlibGateway.generated.h"

UCLASS()
class RLLIB_UE5_DEMO_API ARLlibGateway : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARLlibGateway();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RLlib")
	FString Address;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RLlib")
	int32 Port = 5556;

	UFUNCTION(BlueprintCallable, Category = "RLlib")
	int32 GetAction(float prev_reward, const TArray<float>& next_observation);

	UFUNCTION(BlueprintCallable, Category = "RLlib")
	void EpisodeDone(float final_reward, const TArray<float>& final_observation, bool is_truncated);

	// TEST: Called every frame
	//virtual void Tick(float DeltaTime) override;

	void PrintToConsole(FString Str, bool Error, bool display);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// Avoid using pybind11 here in this header file at all cost. pybind11 defines some macros that clash
	// with UE5's own macros, e.g. "check" and lead to uncomprehensible compile errors due to
	// produced garbage code. We'll reinterpret-cast GatewayInstance back to a py::object before
	// calling its `get_action` method.
	void* GatewayInstance;

	// Helper method to handle a single step in the env, whether it's the first, some in the middle, or the
	// last one at the end of the episode.
	int32 EnvStepHelper(float reward, const TArray<float>& observation, bool terminated, bool truncated);
};

