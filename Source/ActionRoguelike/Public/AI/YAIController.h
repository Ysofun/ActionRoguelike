// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "YAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AYAIController : public AAIController
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviourTree;

	virtual void BeginPlay() override;
};
