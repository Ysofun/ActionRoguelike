// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "YAICharacter.generated.h"

class UPawnSensingComponent;
class UYAttributeComponent;
class UUserWidget;
class UYWorldUserWidget;
class UYActionComponent;

UCLASS()
class ACTIONROGUELIKE_API AYAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AYAICharacter();

protected:

	UYWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpottedWidgetClass;

	void SetTargetActor(AActor* NewTarget);

	APawn* GetTargetActor() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UYAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UYActionComponent* ActionComp;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UYAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;
};
