// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YPowerUpActor.h"
#include "YPowerUp_HealthPotion.generated.h"


/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AYPowerUp_HealthPotion : public AYPowerUpActor
{
	GENERATED_BODY()
	
public:

	AYPowerUp_HealthPotion();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Credtis")
	int32 CreditsCost;

	void Interact_Implementation(APawn* InstigatorPawn) override;
};
