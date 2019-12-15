// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"

void ASProjectileWeapon::Fire()
{
  AActor* MyOwner = GetOwner();
  if (MyOwner && ProjectileClass && AmmoCount > 0)
  {
    FVector EyeLocation;
    FRotator EyeRotation;

    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
    FVector ShotDirection = EyeRotation.Vector();
    
    FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
    FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);

    PlayerFireEffects();

    AmmoCount--;
  }
}
