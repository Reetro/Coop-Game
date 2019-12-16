// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class USHealthComponent;
class UStaticMeshComponent;
class URadialForceComponent;
class UParticleSystem;

UCLASS()
class COOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  UPROPERTY(VisibleAnywhere, Category = "Components")
  USHealthComponent* HealthComp;

  UPROPERTY(VisibleAnywhere, Category = "Components")
  UStaticMeshComponent* MeshComp;

  UPROPERTY(VisibleAnywhere, Category = "Components")
  URadialForceComponent* RadialForceComp;

  UFUNCTION()
  void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

  UPROPERTY(ReplicatedUsing = OnRep_Exploded)
  bool bExploded;

  UFUNCTION()
  void OnRep_Exploded();

  UPROPERTY(EditDefaultsOnly, Category = "FX")
  float ExplosionImpluse;

  UPROPERTY(EditDefaultsOnly, Category = "FX")
  UParticleSystem* ExplosionEffect;

  UPROPERTY(EditDefaultsOnly, Category = "FX")
  UMaterialInterface* ExplodedMaterial;

  UPROPERTY(EditDefaultsOnly, Category = "Damage")
  float RadialDamage;

  UPROPERTY(EditDefaultsOnly, Category = "Damage")
  float DamageRadius;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
  USoundBase* ExplosionSound;

public:	

  // Sets default values for this actor's properties
  ASExplosiveBarrel();
};
