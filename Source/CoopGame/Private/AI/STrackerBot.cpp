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


static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(
  TEXT("COOP.DebugTrackerBot"),
  DebugTrackerBotDrawing,
  TEXT("Draw Debug Lines for TrackerBot"),
  ECVF_Cheat);


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
  ExplosionDamage = 25;
  ExplosionRadius = 35;
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

    FTimerHandle TimerHandle_CheckPower;
    GetWorldTimerManager().SetTimer(TimerHandle_CheckPower, this, &ASTrackerBot::OnCheckNearbyBots, 1.0f, true);
  }

  HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandelTackDamage);
}

FVector ASTrackerBot::GetNextPathPoint()
{
  AActor* BestTarget = nullptr;
  float NearestTargetDistance = FLT_MAX;

  for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
  {
    APawn* TestPawn = It->Get();
    if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
    {
      continue;
    }

    USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
    if (TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.0f)
    {
      float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();

      if (Distance < NearestTargetDistance)
      {
        BestTarget = TestPawn;
        NearestTargetDistance = Distance;
      }
    }
  }

  if (BestTarget)
  {
    UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);

    GetWorldTimerManager().ClearTimer(TimerHandle_RefreshPath);
    GetWorldTimerManager().SetTimer(TimerHandle_RefreshPath, this, &ASTrackerBot::RefreshPath, 5.0f, false);

    if (NavPath && NavPath->PathPoints.Num() > 1)
    {
      return NavPath->PathPoints[1];
    }
  }
  
  // Failed to get target
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

    float ActualDamage = ExplosionDamage + (ExplosionDamage * PowerLevel);

    UGameplayStatics::ApplyRadialDamage(GetWorld(), ActualDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

    if (DebugTrackerBotDrawing)
    {
      DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 5.0f, 0, 1.0f);

    }
   
    SetLifeSpan(2.0f);
  }
}

void ASTrackerBot::DamageSelf()
{
  UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::RefreshPath()
{
  NextPathPoint = GetNextPathPoint();
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

      if (DebugTrackerBotDrawing)
      {
        DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached");
      }
    }
    else
    {
      // Keep moving to next point 
      FVector ForceDirection = NextPathPoint - GetActorLocation();
      ForceDirection.Normalize();

      ForceDirection *= MovementForce;

      MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

      if (DebugTrackerBotDrawing)
      {
        DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Red, false, 0.0f, 0, 1.0f);
      }
    }

    if (DebugTrackerBotDrawing)
    {
      DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Red, false, 0.0f, 1.0f);
    }

  }
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
  Super::NotifyActorBeginOverlap(OtherActor);

  if (!bStartedSelfDestruction && !bExploded)
  {
    ASCharacter* OverlappedActor = Cast<ASCharacter>(OtherActor);
    if (OverlappedActor && !USHealthComponent::IsFriendly(OtherActor, this))
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

void ASTrackerBot::OnCheckNearbyBots()
{
  // distance to check for nearby bots
  const float Radius = 600;

  // Temp collision
  FCollisionShape CollShape;
  CollShape.SetSphere(Radius);

  FCollisionObjectQueryParams QueryParams;

  QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
  QueryParams.AddObjectTypesToQuery(ECC_Pawn);

  TArray<FOverlapResult> Overlaps;
  GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

  if (DebugTrackerBotDrawing)
  {
    DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, FColor::White, false, 1.0f);
  }

  int32 NumberOfBots = 0;
  for (FOverlapResult Result : Overlaps)
  {
    ASTrackerBot* Bot = Cast<ASTrackerBot>(Result.GetActor());

    if (Bot && Bot != this)
    {
      NumberOfBots++;
    }
  }

  const int32 MaxPowerLevel = 4;

  PowerLevel = FMath::Clamp(NumberOfBots, 0, MaxPowerLevel);

  if (MatInst == nullptr)
  {
    MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
  }

  if (MatInst)
  {
    float Alpha = PowerLevel / (float)MaxPowerLevel;
    MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
  }

  if (DebugTrackerBotDrawing)
  {
    // Draw on the bot location
    DrawDebugString(GetWorld(), FVector(0, 0, 0), FString::FromInt(PowerLevel), this, FColor::White, 1.0f, true);
  }
}
