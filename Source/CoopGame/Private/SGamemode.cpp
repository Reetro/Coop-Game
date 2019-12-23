// Fill out your copyright notice in the Description page of Project Settings.


#include "SGamemode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "SGameState.h"
#include "SPlayerState.h" 
#include "SHealthComponent.h"
#include "GameFramework/Actor.h"

ASGamemode::ASGamemode()
{
  TimeBetweenWaves = 2.0f;

  GameStateClass = ASGameState::StaticClass();
  PlayerStateClass = ASPlayerState::StaticClass();

  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.TickInterval = 1.0f;
}

void ASGamemode::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);

  CheckWaveState();
  CheckAnyPlayerAlive();
}

void ASGamemode::StartWave()
{
  WaveCount++;

  NumOfBotsToSpawn = 2 * WaveCount;

  GetWorldTimerManager().SetTimer(TimerHandel_BotSpawner, this, &ASGamemode::SpawnBotTimerElasped, 1.0f, true, 0.0f);

  SetWaveState(EWaveState::WaveInProgress);
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

    SetWaveState(EWaveState::WaveComplete);
  }
}

void ASGamemode::CheckAnyPlayerAlive()
{

  for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
  {
    APlayerController* PC = It->Get();
    if (PC && PC->GetPawn())
    {
      APawn* MyPawn = PC->GetPawn();
      USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
      if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
      {
        // Player is alive
        return;
      }
    }
  }

  // All players are dead
  GameOver();
}

void ASGamemode::GameOver()
{
  EndWave();

  SetWaveState(EWaveState::GameOver);
}

void ASGamemode::SetWaveState(EWaveState NewState)
{
  ASGameState* GS = GetGameState<ASGameState>();
  if (ensureAlways(GS))
  {
    GS->SetWaveState(NewState);
  }
}

void ASGamemode::RestartDeadPlayers()
{
  for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
  {
    APlayerController* PC = It->Get();
    if (PC && PC->GetPawn() == nullptr)
    {
      RestartPlayer(PC);
    }
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

  SetWaveState(EWaveState::WaitingToCompelete);
}

void ASGamemode::PrepareForNextWave()
{
  GetWorldTimerManager().SetTimer(TimerHandel_NextWaveStart, this, &ASGamemode::StartWave, TimeBetweenWaves, false);

  SetWaveState(EWaveState::WaitingToStart);

  RestartDeadPlayers();
}
