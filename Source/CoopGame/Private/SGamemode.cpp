// Fill out your copyright notice in the Description page of Project Settings.


#include "SGamemode.h"
#include "TimerManager.h"


ASGamemode::ASGamemode()
{
  TimeBetweenWaves = 2.0f;
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

void ASGamemode::StartPlay()
{
  Super::StartPlay();

  PrepareForNextWave();
}

void ASGamemode::EndWave()
{
  GetWorldTimerManager().ClearTimer(TimerHandel_BotSpawner);

  PrepareForNextWave();
}

void ASGamemode::PrepareForNextWave()
{
  FTimerHandle TimerHandel_NextWaveStart;

  GetWorldTimerManager().SetTimer(TimerHandel_NextWaveStart, this, &ASGamemode::StartWave, TimeBetweenWaves, false);
}
