// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
  HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

  MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
  RootComponent = MeshComp;

  MeshComp->SetSimulatePhysics(true);
  MeshComp->SetCollisionObjectType(ECC_PhysicsBody);

  RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
  RadialForceComp->SetupAttachment(MeshComp);
  RadialForceComp->Radius = 250;
  RadialForceComp->bImpulseVelChange = true;
  RadialForceComp->bAutoActivate = false;
  RadialForceComp->bIgnoreOwningActor = true;

  ExplosionImpluse = 400;

  RadialDamage = 20;
  DamageRadius = 200;
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
  HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
  if (bExploded)
  {
    // Already exploded
    return;
  }

  if (Health <= 0)
  {
    // Exploding
    bExploded = true;

    // Launch Upwards
    FVector BoostIntesity = FVector::UpVector * ExplosionImpluse;
    MeshComp->AddImpulse(BoostIntesity, NAME_None, true);

    // Spawn FX
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
    // Change Material
    MeshComp->SetMaterial(0, ExplodedMaterial);

    // Apply radial force to blast away actors in range
    RadialForceComp->FireImpulse();

    // Apply radial damage to actors in range
    UGameplayStatics::ApplyRadialDamage(
      this,
      RadialDamage,
      GetActorLocation(),
      DamageRadius,
      UDamageType::StaticClass(),
      TArray<AActor*>() // damage all actors
    );
  }
}
