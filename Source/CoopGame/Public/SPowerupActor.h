// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor(); 

protected:

  UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
  float PowerupInterval;

  UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
  int32 NumberOfTicks;

  FTimerHandle TimerHandle_PowerupTick;
  
  // Number of ticks applied
  int32 TicksProcessed;

  UFUNCTION()
  void OnTickPowerup();

  UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
  bool bIsPowerupActive;

  UFUNCTION()
  void OnRep_PowerupActive();

  UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
  void OnPowerupStateChange(bool bNewIsActive);

public:

  void ActivatePowerup(AActor* ActiveFor);

  UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
  void OnActivate(AActor* ActiveFor);

  UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
  void OnPowerUpTicked();

  UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
  void OnExpired();
};
