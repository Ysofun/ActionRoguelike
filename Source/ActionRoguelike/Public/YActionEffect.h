// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "YAction.h"
#include "YActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UYActionEffect : public UYAction
{
	GENERATED_BODY()
	
public:

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Duration;

	// Time between "ticks" to apply effect.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Period;

	FTimerHandle TimerHandle_Duration;
	FTimerHandle TimerHandle_Period;

	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodEffect(AActor* Instigator);

public:

	UFUNCTION(BlueprintCallable)
	float GetTimeRemaining() const;

	UYActionEffect();
};
