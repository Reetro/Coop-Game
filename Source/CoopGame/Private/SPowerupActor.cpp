// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
  PowerupInterval = 0.0f;
  NumberOfTicks = 0;
  bIsPowerupActive = false;

  SetReplicates(true);
}

void ASPowerupActor::OnTickPowerup()
{
  TicksProcessed++;

  OnPowerUpTicked();

  if (TicksProcessed >= NumberOfTicks)
  {
    OnExpired();

    bIsPowerupActive = false;
    OnRep_PowerupActive();

    GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
  }
}

void ASPowerupActor::OnRep_PowerupActive()
{
  OnPowerupStateChange(bIsPowerupActive);
}

void ASPowerupActor::ActivatePowerup(AActor* ActiveFor)
{

  OnActivate(ActiveFor);

  bIsPowerupActive = true;
  OnRep_PowerupActive();

  if (PowerupInterval > 0.0f)
  {
    GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
  }
  else
  {
    OnTickPowerup();
  }
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

  DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}
