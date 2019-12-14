// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
  RootComponent = MeshComp;

  MuzzleSocketName = "MuzzleSocket";
  TracerTargetName = "Target";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

}

void ASWeapon::Fire()
{
  AActor* MyOwner = GetOwner();
  if (MyOwner)
  {
    FVector EyeLocation;
    FRotator EyeRotation;
   
    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
    FVector ShotDirection = EyeRotation.Vector();
    FVector TraceRange = EyeLocation + (ShotDirection * 10000);
    
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(MyOwner);
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = true;

    FVector TracerEndPoint = TraceRange;

    FHitResult Hit;
    if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceRange, ECC_Visibility, QueryParams))
    {
      // Blocking hit
      AActor* HitActor = Hit.GetActor();

      UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

      if (ImpactEffect)
      {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
      }

      TracerEndPoint = Hit.TraceEnd;
    }

    DrawDebugLine(GetWorld(), EyeLocation, TraceRange, FColor::Red, false, 1.0f, 0, 1.0f);

    if (MuzzleEffect)
    {
      UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
    }

    if (TracerEffect)
    {
      FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

      UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
      if (TracerComp)
      {
        TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
      }
    }
  }
}
