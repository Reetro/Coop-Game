// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGamemode.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGamemode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
  // Hook for BP to spawn a single bot
  UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
  void SpawnNewBot();

  // Start bot spawning
  void StartWave();

  // Stop spawning bots
  void EndWave();

  // Timer for next wave
  void PrepareForNextWave();

  void SpawnBotTimerElasped();

  FTimerHandle TimerHandel_BotSpawner;

  FTimerHandle TimerHandel_NextWaveStart;

  // Number of bots to spawn in current wave
  int32 NumOfBotsToSpawn;

  int32 WaveCount;

  UPROPERTY(EditDefaultsOnly, Category = "GameMode")
  float TimeBetweenWaves;

  void CheckWaveState();
  
  void CheckAnyPlayerAlive();

  // It's game over man it's game over
  void GameOver();

public:

  virtual void Tick(float DeltaSeconds) override;

  ASGamemode();

  virtual void StartPlay() override;
};
