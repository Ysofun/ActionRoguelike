// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YPowerUpActor.h"
#include "YPowerUp_Action.generated.h"


class UYAction;


/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AYPowerUpActor_Action : public AYPowerUpActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category = "PowerUp")
	TSubclassOf<UYAction> ActionToGrant;

public:

	void Interact_Implementation(APawn* InstigatorPawn) override;
};
