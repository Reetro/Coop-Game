// Fill out your copyright notice in the Description page of Project Settings.


#include "SAmmoPickup.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SCharacter.h"
#include "SWeapon.h"

// Sets default values
ASAmmoPickup::ASAmmoPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
  RootComponent = BoxCollision;
  BoxCollision->SetBoxExtent(FVector(50));
  BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);

  MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  MeshComp->SetupAttachment(BoxCollision);
  MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  RifleAmmoToAdd = 40;
  LauncherAmmoToAdd = 8;

  SetReplicates(true);
}

void ASAmmoPickup::BeginPlay()
{
  Super::BeginPlay();

  BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASAmmoPickup::OnBeginOverlap);
}

void ASAmmoPickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  
}
