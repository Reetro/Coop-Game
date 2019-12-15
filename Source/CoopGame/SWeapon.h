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
  UParticleSystem* ImpactEffect;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
  UParticleSystem* TracerEffect;

  UPROPERTY(EditDefaultsOnly, Category = "Weapon")
  TSubclassOf<UCameraShake> FireCamShake;

  void PlayerFireEffects(FVector TracerEndPoint);

  void PlayerFireEffects();

public:	
  // Sets default values for this actor's properties
  ASWeapon();

  UFUNCTION(BlueprintCallable, Category = "Weapon")
  virtual void Fire();
};
