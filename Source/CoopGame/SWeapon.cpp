// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for Weapons"), 
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
  MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
  RootComponent = MeshComp;

  MuzzleSocketName = "MuzzleSocket";
  TracerTargetName = "Target";
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

    if (DebugWeaponDrawing > 0)
    {
      DrawDebugLine(GetWorld(), EyeLocation, TraceRange, FColor::Red, false, 1.0f, 0, 1.0f);
    }

    PlayerFireEffects(TracerEndPoint);
  }
}

void ASWeapon::PlayerFireEffects(FVector TracerEndPoint)
{
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
