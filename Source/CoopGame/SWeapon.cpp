// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "CoopGame.h"
#include "TimerManager.h"

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

  BaseDamage = 20.0f;
  HeadShotDamageMultipler = 4;

  RateOfFire = 600;

  AmmoCount = 30;
}

void ASWeapon::StartFire()
{
  float FirstDelay = FMath::Max(LastTimeFired + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

  GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
  GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::Fire()
{
  if (AmmoCount > 0)
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
      QueryParams.bReturnPhysicalMaterial = true;

      FVector TracerEndPoint = TraceRange;

      FHitResult Hit;
      if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceRange, COLLISION_WEAPON, QueryParams))
      {
        // Blocking hit
        AActor* HitActor = Hit.GetActor();

        EPhysicalSurface Surface = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

        float ActualDamage = BaseDamage;

        if (Surface == SURFACE_FLESHVULNERABLE)
        {
          ActualDamage *= HeadShotDamageMultipler;
        }

        UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

        TracerEndPoint = Hit.TraceEnd;

        UParticleSystem* SelectedEffect = nullptr;

        switch (Surface)
        {
        case SurfaceType_Default:
          SelectedEffect = DefaultImpactEffect;
          break;
        case SURFACE_FLESHDEFAULT:
        case SURFACE_FLESHVULNERABLE:
          SelectedEffect = FleshImpactEffect;
          break;
        }

        if (SelectedEffect)
        {
          UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
        }

      }

      if (DebugWeaponDrawing > 0)
      {
        DrawDebugLine(GetWorld(), EyeLocation, TraceRange, FColor::Red, false, 1.0f, 0, 1.0f);
      }

      PlayerFireEffects(TracerEndPoint);

      LastTimeFired = GetWorld()->TimeSeconds;

      AmmoCount--;
    }
  }
}

void ASWeapon::BeginPlay()
{
  Super::BeginPlay();

  TimeBetweenShots = 60 / RateOfFire;
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

  APawn* MyOwner = Cast<APawn>(GetOwner());
  if (MyOwner)
  {
    APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
    if (PC)
    {
      PC->ClientPlayCameraShake(FireCamShake);
    }
  }
}

void ASWeapon::PlayerFireEffects()
{
  APawn* MyOwner = Cast<APawn>(GetOwner());
  if (MyOwner)
  {
    APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
    if (PC)
    {
      PC->ClientPlayCameraShake(FireCamShake);
    }

    if (MuzzleEffect)
    {
      UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
    }
  }
}
