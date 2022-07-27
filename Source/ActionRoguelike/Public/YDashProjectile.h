// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YProjectileBase.h"
#include "YDashProjectile.generated.h"

UCLASS()
class ACTIONROGUELIKE_API AYDashProjectile : public AYProjectileBase
{
	GENERATED_BODY()

public:

	AYDashProjectile();
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	FTimerHandle TimerHandle_DelayDetonate;

	virtual void Explode_Implementation() override;

	void TeleportInstigator();

	virtual void BeginPlay() override;
};
