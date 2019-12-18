// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, OwningHealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()
    
public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

  UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category = "HealthCompoent")
  float Health;	

  UFUNCTION()
  void OnRep_Health(float OldHealth);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthCompoent")
  float DefaultHealth;

  UFUNCTION()
  void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

  bool bIsDead;

public:

  float GetHealth() const;

  UPROPERTY(BlueprintAssignable, Category = "Health Events")
  FOnHealthChangedSignature OnHealthChanged;

  UFUNCTION(BlueprintCallable, Category = "HealthCompoent")
  void Heal(float HealAmount);
};
