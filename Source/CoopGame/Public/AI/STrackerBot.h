// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USHealthComponent;
class USphereComponent;
class UStaticMeshComponent;
class UParticleSystem;
class UMaterialInstanceDynamic;
class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
  
  UPROPERTY(VisibleDefaultsOnly, Category = "Components")
  UStaticMeshComponent* MeshComp;

  UPROPERTY(VisibleDefaultsOnly, Category = "Components")
  USHealthComponent* HealthComp;

  UPROPERTY(VisibleDefaultsOnly, Category = "Components")
  USphereComponent* SphereComp;

  UMaterialInstanceDynamic* MatInst;

  FVector GetNextPathPoint();

  FVector NextPathPoint;

  bool bExploded;

  bool bStartedSelfDestruction;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  float MovementForce;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  bool bUseVelocityChange;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  float RequiredDistanceToTarget;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  float ExplosionRadius;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  float ExplosionDamage;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  float SelfDamageInterval;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  UParticleSystem* ExplosionEffect;

  UFUNCTION()
  void HandelTackDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

  void SelfDestruct();

  void OnCheckNearbyBots();

  FTimerHandle TimerHandel_SelfDamage;

  void DamageSelf();

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  USoundCue* SelfDestructSound;

  UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
  USoundCue* ExplosionSound;

  int32 PowerLevel;

  void RefreshPath();

  FTimerHandle TimerHandle_RefreshPath;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

  virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
