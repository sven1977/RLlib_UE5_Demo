// Fill out your copyright notice in the Description page of Project Settings.

#include "RLlibGateway.h"

#undef check
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;
static_assert(sizeof(void*) >= sizeof(py::object), "void* too small for py::object");

static std::unique_ptr<py::scoped_interpreter> PythonInterp;

// Sets default values
ARLlibGateway::ARLlibGateway()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true; // TEST
  PrimaryActorTick.bStartWithTickEnabled = true;
  PrimaryActorTick.TickInterval = 0.0f;
  PrimaryActorTick.TickGroup = TG_PrePhysics;
  //SetActorTickEnabled(true);
  if (!PythonInterp) {
    PythonInterp = std::make_unique<py::scoped_interpreter>();
  }
}

// Called when the game starts or when spawned
void ARLlibGateway::BeginPlay()
{
  Super::BeginPlay();

  // Enable ticking.
  SetActorTickEnabled(true);
  //SetTickableWhenPaused(true);

  try {
    py::gil_scoped_acquire gil;

    py::module_ sys = py::module_::import("sys");
    //sys.attr("path").attr("append")("C:/Programs/Anaconda3/envs/simulator/Lib/site-packages");

    py::module_ gateway_mod = py::module_::import("ray.rllib.env.external.rllib_gateway");
    py::object gateway_cls = gateway_mod.attr("RLlibGateway");
    GatewayInstance = new py::object(gateway_cls(
      TCHAR_TO_UTF8(*Address),
      Port,
      std::string("INFO") // python log level
    ));
  }
  catch (...) {
    // Catch all other unknown exceptions
    PrintToConsole(TEXT("Exception caught while trying to setup RLlibGateway instance in python."), false, false);
  }
}

void ARLlibGateway::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  py::gil_scoped_acquire gil;
  auto* pyObj = reinterpret_cast<py::object*>(GatewayInstance);
  py::object& gateway = *pyObj;

  gateway.attr("cleanup")();
}

// Called every frame
//void ARLlibGateway::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//  //int action = GetAction(0.1f, TArray<float>{ 0.0f, 0.1f, 0.2f, 0.3f }, false, false);
//  //return action;
//  PrintToConsole(FString::Printf(TEXT("Computed action %i"), 2), false, true);
//}

int32 ARLlibGateway::GetAction(float prev_reward, const TArray<float>& next_observation) {
  // Perform a single (non-terminal/truncated) step with an action computation.
  // The returned action will be used in at least yet one other step in the future.
  int32 action = EnvStepHelper(prev_reward, next_observation, false, false);
  //PrintToConsole(FString::Printf(TEXT("Computed action %i"), action), false, false);
  return action;
}

void ARLlibGateway::EpisodeDone(float final_reward, const TArray<float>& final_observation, bool is_truncated) {
  // Perform a last step in the env and log the done-type (terminated or truncated). This last step includes
  // a hidden action computation to force an env-to-module pass for possible observation preprocessing.
  EnvStepHelper(final_reward, final_observation, !is_truncated, is_truncated);
  return;
}

int32 ARLlibGateway::EnvStepHelper(float reward, const TArray<float>& observation, bool terminated, bool truncated) {

  py::gil_scoped_acquire gil;
  std::vector<float> obsVec(observation.GetData(), observation.GetData() + observation.Num());

  auto* pyObj = reinterpret_cast<py::object*>(GatewayInstance);
  py::object& gateway = *pyObj;

  if (!gateway.attr("is_initialized").cast<bool>())
  {
    return -1;
  }
  else if (terminated || truncated) {
    gateway.attr("episode_done")(reward, obsVec, truncated);
    return -1;
  }
  else
  {
    py::object action = gateway.attr("get_action")(reward, obsVec);
    //int timesteps = gateway.attr("timesteps").cast<int>();
    //if (timesteps % 100 == 0)
    //{
    //  PrintToConsole(FString::Printf(TEXT("timesteps = %i"), timesteps), false, false);
    //}
    return action.cast<int>();
  }
}

void ARLlibGateway::PrintToConsole(FString Str, bool Error, bool display)
{
  FDateTime timestamp = FDateTime::UtcNow();
  if (Error)
  {
    auto messageLog = FMessageLog("RLlibClientPlugin");
    messageLog.Open(EMessageSeverity::Error, true);
    messageLog.Message(EMessageSeverity::Error, FText::AsCultureInvariant(Str));
#if UE_BUILD_DEVELOPMENT
    if (display && GEngine)
    {
      GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s"), *Str));
    }
#endif
  }
  else
  {
#if UE_BUILD_DEVELOPMENT
    if (display && GEngine)
    {
      GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("%s"), *Str));
    }
#endif
    UE_LOG(LogTemp, Log, TEXT("Log: %s"), *Str);
  }
}
