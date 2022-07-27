// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YPowerUpActor.h"
#include "YPowerUp_Credits.generated.h"


/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AYPowerUp_Credits : public AYPowerUpActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int32 CreditsAmount;

	void Interact_Implementation(APawn* InstigatorPawn) override;

public:

	AYPowerUp_Credits();
};
