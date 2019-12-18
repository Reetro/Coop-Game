// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SAmmoPickup.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASAmmoPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASAmmoPickup();

protected:

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
  UStaticMeshComponent* MeshComp;
  
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
  UBoxComponent* BoxCollision;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
  USoundCue* PickUpSound;

  virtual void BeginPlay() override;

  UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
  int32 RifleAmmoToAdd;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
  int32 LauncherAmmoToAdd;
};
