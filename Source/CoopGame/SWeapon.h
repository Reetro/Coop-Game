// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

protected:

  virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  USkeletalMeshComponent* MeshComp;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  TSubclassOf<UDamageType> DamageType;

  UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  FName MuzzleSocketName;

  UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  FName TracerTargetName;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  UParticleSystem* MuzzleEffect;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  UParticleSystem* DefaultImpactEffect;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  UParticleSystem* FleshImpactEffect;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  UParticleSystem* TracerEffect;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0))
  int32 CurrentAmmoCount;

  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  TSubclassOf<UCameraShake> FireCamShake;

  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  float BaseDamage;

  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  float HeadShotDamageMultipler;

  // RPM - Bullets per minute fired by weapon
  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  float RateOfFire;

  void PlayerFireEffects(FVector TracerEndPoint);

  void PlayerFireEffects();

  virtual void Fire();

  FTimerHandle TimerHandle_TimeBetweenShots;

  float LastTimeFired;

  float TimeBetweenShots;

public:	
  // Sets default values for this actor's properties
  ASWeapon();

  void StartFire();

  void StopFire();

  void AddToAmmoCount(int32 AmountToAdd);
};
