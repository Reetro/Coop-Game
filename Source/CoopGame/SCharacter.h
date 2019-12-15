// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;


UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
  GENERATED_BODY()

public:
  // Sets default values for this character's properties
  ASCharacter();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  void MoveFoward(float Value);

  void MoveRight(float Value);

  void BeginCrouch();

  void EndCrouch();

  void BeginZoom();

  void EndZoom();

  void StartFire();

  void StopFire();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  UCameraComponent* CameraComp = nullptr;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  USpringArmComponent* SpringArmComp = nullptr;

  bool bWantsToZoom;

  UPROPERTY(EditDefaultsOnly, Category = "Player")
  float ZoomFOV;

  UPROPERTY(EditDefaultsOnly, Category = "Player")
  TSubclassOf<ASWeapon> StaterWeaponClass;

  UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
  float ZoomInterpSpeed;

  UPROPERTY(EditDefaultsOnly, Category = "Player")
  FName WeaponAttachSocketName;

  float DefaultFOV;

  ASWeapon* CurrentWeapon;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  // Called to bind functionality to input
  virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

  virtual FVector GetPawnViewLocation() const override;
};
