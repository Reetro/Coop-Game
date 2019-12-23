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
#include "Net/UnrealNetwork.h"
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
  BulletSpread = 1.0f;

  RateOfFire = 600;

  bUseAmmoCount = true;
  CurrentAmmoCount = 0;   

  SetReplicates(true);

  NetUpdateFrequency = 66.0f;
  MinNetUpdateFrequency = 33.0f;
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
  if (CurrentAmmoCount > 0 || !bUseAmmoCount)
  {
    
    // Trace the world, from pawn eyes to crosshair location

    if (Role < ROLE_Authority)
    {
      ServerFire();
    }

    AActor* MyOwner = GetOwner();
    if (MyOwner)
    {
      FVector EyeLocation;
      FRotator EyeRotation;
      MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

      FVector ShotDirection = EyeRotation.Vector();

      // Bullet Spread
      float HalfRad = FMath::DegreesToRadians(BulletSpread);
      ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
   
      FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

      FCollisionQueryParams QueryParams;
      QueryParams.AddIgnoredActor(MyOwner);
      QueryParams.AddIgnoredActor(this);
      QueryParams.bTraceComplex = true;
      QueryParams.bReturnPhysicalMaterial = true;

      // Particle "Target" parameter
      FVector TracerEndPoint = TraceEnd;

      EPhysicalSurface SurfaceType = SurfaceType_Default;

      FHitResult Hit;
      if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
      {
        // Blocked Hit
        AActor* HitActor = Hit.GetActor();

        SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

        float ActualDamage = BaseDamage;
        if (SurfaceType == SURFACE_FLESHVULNERABLE)
        {
          ActualDamage *= 4.0f;
        }

        UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);

        PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

        TracerEndPoint = Hit.ImpactPoint;

      }

      if (DebugWeaponDrawing > 0)
      {
        DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
      }

      PlayFireEffects(TracerEndPoint);

      if (Role == ROLE_Authority)
      {
        HitScanTrace.TraceTo = TracerEndPoint;
        HitScanTrace.SurfaceType = SurfaceType;
      }
     
      if (FireSound)
      {
        UGameplayStatics::SpawnSoundAttached(FireSound, RootComponent);
      }
     
      LastTimeFired = GetWorld()->TimeSeconds;
    }

    CurrentAmmoCount--;

  }
}


void ASWeapon::OnRep_HitScanTrace()
{
  // Play FX
  PlayFireEffects(HitScanTrace.TraceTo);
  PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::ServerFire_Implementation()
{
  Fire();
}

bool ASWeapon::ServerFire_Validate()
{
  return true;
}

void ASWeapon::BeginPlay()
{
  Super::BeginPlay();

  TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint)
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

void ASWeapon::PlayFireEffects()
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

void ASWeapon::AddToLocalAmmoCount(int32 AmountToAdd)
{
  CurrentAmmoCount += AmountToAdd;
}

void ASWeapon::AddToPlayerAmmoCount(int32 RifleAmmoToAdd, int32 LauncherAmmoToAdd)
{
  switch (Weapontype)
  {
  case EWeaponType::Rifle:
    AddToLocalAmmoCount(RifleAmmoToAdd);
    break;
  case EWeaponType::Launcher:
    AddToLocalAmmoCount(LauncherAmmoToAdd);
    break;
  default:
    AddToLocalAmmoCount(RifleAmmoToAdd);
    break;
  }
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
  UParticleSystem* SelectedEffect = nullptr;
  switch (SurfaceType)
  {
  case SURFACE_FLESHDEFAULT:
  case SURFACE_FLESHVULNERABLE:
    SelectedEffect = FleshImpactEffect;
    break;
  default:
    SelectedEffect = DefaultImpactEffect;
    break;
  }

  if (SelectedEffect)
  {
    FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

    FVector ShotDirection = ImpactPoint - MuzzleLocation;
    ShotDirection.Normalize();

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
  }
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  Super::GetLifetimeReplicatedProps(OutLifetimeProps);

  DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}
