// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;
class USoundBase;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
  Rifle,
  Launcher
};

USTRUCT()
struct FHitScanTrace
{
  GENERATED_BODY()

public:

  UPROPERTY()
  TEnumAsByte<EPhysicalSurface> SurfaceType;

  UPROPERTY()
  FVector_NetQuantize TraceTo;
};


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

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  USoundBase* FireSound;

  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  float BaseDamage;

  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  float HeadShotDamageMultipler;

  // RPM - Bullets per minute fired by weapon
  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  float RateOfFire;

  void PlayFireEffects(FVector TracerEndPoint);

  void PlayFireEffects();

  virtual void Fire();

  void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

  UFUNCTION(Server, Reliable, WithValidation)
  void ServerFire();

  FTimerHandle TimerHandle_TimeBetweenShots;

  float LastTimeFired;

  float TimeBetweenShots;
  

  UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
  FHitScanTrace HitScanTrace;

  UFUNCTION()
  void OnRep_HitScanTrace();

public:	
  // Sets default values for this actor's properties
  ASWeapon();

  void StartFire();

  void StopFire();

  void AddToLocalAmmoCount(int32 AmountToAdd);

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
  EWeaponType Weapontype;

  UFUNCTION(BlueprintCallable, Category = "Weapon")
  void AddToPlayerAmmoCount(int32 RifleAmmoToAdd, int32 LauncherAmmoToAdd);

};
