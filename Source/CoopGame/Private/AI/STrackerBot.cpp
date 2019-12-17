// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/NavigationSystemBase.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
  MeshComp->SetCanEverAffectNavigation(false);
  MeshComp->SetSimulatePhysics(true);
  RootComponent = MeshComp;

  bUseVelocityChange = true;
  MovementForce = 1000;
  RequiredDistanceToTarget = 100;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
  // Get first point to move to
  NextPathPoint = GetNextPathPoint();
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

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
