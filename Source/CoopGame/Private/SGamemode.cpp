// Fill out your copyright notice in the Description page of Project Settings.


#include "SGamemode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SHealthComponent.h"
#include "GameFramework/Actor.h"

ASGamemode::ASGamemode()
{
  TimeBetweenWaves = 2.0f;

  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1.0f;
}

void ASGamemode::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);

  CheckWaveState();
}

void ASGamemode::StartWave()
{
  WaveCount++;

  NumOfBotsToSpawn = 2 * WaveCount;

  GetWorldTimerManager().SetTimer(TimerHandel_BotSpawner, this, &ASGamemode::SpawnBotTimerElasped, 1.0f, true, 0.0f);
}

void ASGamemode::SpawnBotTimerElasped()
{
  SpawnNewBot();

  NumOfBotsToSpawn--;

  if (NumOfBotsToSpawn <= 0)
  {
    EndWave();
  }
}

void ASGamemode::CheckWaveState()
{
  bool bIsPreparingForNextWave = GetWorldTimerManager().IsTimerActive(TimerHandel_NextWaveStart);

  if (NumOfBotsToSpawn > 0 || bIsPreparingForNextWave)
  {
    return;
  }

  bool bIsAnyBotAlive = false;

  for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
  {
    APawn* TestPawn = It->Get();
    if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
    {
      continue;
    }

    USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
    if (HealthComp && HealthComp->GetHealth() > 0.0f)
    {
      bIsAnyBotAlive = true;
      break;
    }
  }

  if (!bIsAnyBotAlive)
  {
    PrepareForNextWave();
  }
}

void ASGamemode::StartPlay()
{
  Super::StartPlay();

  PrepareForNextWave();
}

void ASGamemode::EndWave()
{
  GetWorldTimerManager().ClearTimer(TimerHandel_BotSpawner);
}

void ASGamemode::PrepareForNextWave()
{
  GetWorldTimerManager().SetTimer(TimerHandel_NextWaveStart, this, &ASGamemode::StartWave, TimeBetweenWaves, false);
}
