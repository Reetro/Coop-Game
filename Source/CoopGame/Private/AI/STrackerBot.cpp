// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SHealthComponent.h"
#include "TimerManager.h"
#include "SCharacter.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
  MeshComp->SetCanEverAffectNavigation(false);
  MeshComp->SetSimulatePhysics(true);
  RootComponent = MeshComp;

  HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

  SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
  SphereComp->SetSphereRadius(200);
  SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
  SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
  SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
  SphereComp->SetupAttachment(MeshComp);

  bUseVelocityChange = true;
  MovementForce = 1000;
  RequiredDistanceToTarget = 100;
  ExplosionDamage = 30;
  ExplosionRadius = 200;
  SelfDamageInterval = 0.25f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

  if (Role == ROLE_Authority)
  {
    // Get first point to move to
    NextPathPoint = GetNextPathPoint();
  }

  HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandelTackDamage);
}

FVector ASTrackerBot::GetNextPathPoint()
{
  ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

  UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

  if (NavPath->PathPoints.Num() > 1)
  {
    return NavPath->PathPoints[1];
  }

  // Unable to find path
  return GetActorLocation();
}

void ASTrackerBot::HandelTackDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

  // Pulse on Hit
  if (MatInst == nullptr)
  {
    MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
  }

  if (MatInst)
  {
    MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
  }

  if (Health <= 0)
  {
    SelfDestruct();
  }
}

void ASTrackerBot::SelfDestruct()
{
  if (bExploded)
  {
    return;
  }

  bExploded = true;

  if (ExplosionEffect)
  {
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
  }

  UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

  MeshComp->SetVisibility(false, true);
  MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  MeshComp->SetSimulatePhysics(false);

  if (Role == ROLE_Authority)
  {
    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this);

    UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

    DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 5.0f, 0, 1.0f);

    SetLifeSpan(2.0f);
  }
}

void ASTrackerBot::DamageSelf()
{
  UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

  if (Role == ROLE_Authority && !bExploded)
  {

    float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

    if (DistanceToTarget <= RequiredDistanceToTarget)
    {
      // Update to new point
      NextPathPoint = GetNextPathPoint();

      DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached");
    }
    else
    {
      // Keep moving to next point 
      FVector ForceDirection = NextPathPoint - GetActorLocation();
      ForceDirection.Normalize();

      ForceDirection *= MovementForce;

      MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

      DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Red, false, 0.0f, 0, 1.0f);
    }

    DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Red, false, 0.0f, 1.0f);
  }
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
  if (!bStartedSelfDestruction && !bExploded)
  {
    ASCharacter* OverlappedActor = Cast<ASCharacter>(OtherActor);
    if (OverlappedActor)
    {

      if (Role == ROLE_Authority)
      {
        // Start Self Destruct
        GetWorldTimerManager().SetTimer(TimerHandel_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
      }

      if (SelfDestructSound)
      {
        UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
      }
    }
  }
}
