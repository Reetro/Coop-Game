// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "SPowerupActor.h"
#include "TimerManager.h"
#include "SCharacter.h"
#include "Components/DecalComponent.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
  SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
  SphereComp->SetSphereRadius(75.0f);
  RootComponent = SphereComp;

  DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
  DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
  DecalComp->DecalSize = FVector(64, 75, 75);
  DecalComp->SetupAttachment(RootComponent);

  CoolDownDuration = 10.0f;

  SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
  if (Role == ROLE_Authority)
  {
    Respawn();
  }
}

void ASPickupActor::Respawn()
{
  if (PowerupClass == nullptr)
  {
    UE_LOG(LogTemp, Warning, TEXT("Hey tard you forgot to set a Powerup class in %s update the blueprint idoit"), *GetName())
    return;
  }

  FActorSpawnParameters SpawnParms;
  SpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

  PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass, GetTransform(), SpawnParms);

  UE_LOG(LogTemp, Log, TEXT("Test"))
}

void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
  Super::NotifyActorBeginOverlap(OtherActor);

  auto PlayerPawn = Cast<ASCharacter>(OtherActor);

  if (Role == ROLE_Authority && PowerupClass && PlayerPawn->IsLocallyControlled())
  {
    if (PowerupInstance)
    {
      PowerupInstance->ActivatePowerup(PlayerPawn);
      PowerupInstance = nullptr;
    }

    GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickupActor::Respawn, CoolDownDuration);
  }
}
