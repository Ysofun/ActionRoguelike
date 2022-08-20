// Fill out your copyright notice in the Description page of Project Settings.


#include "YPowerUp_HealthPotion.h"
#include "YAttributeComponent.h"
#include "YPlayerState.h"


#define LOCTEXT_NAMESPACE "InteractableActors"


AYPowerUp_HealthPotion::AYPowerUp_HealthPotion()
{
	CreditsCost = 50;
}

void AYPowerUp_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	UYAttributeComponent* AttributeComp = Cast<UYAttributeComponent>(InstigatorPawn->GetComponentByClass(UYAttributeComponent::StaticClass()));

	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		if (AYPlayerState* PS = InstigatorPawn->GetPlayerState<AYPlayerState>())
		{
			if (PS->RemoveCredits(CreditsCost) && AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthMax()))
			{
				HideAndCooldownPowerUp();
			}
		}
	}
}

FText AYPowerUp_HealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	UYAttributeComponent* AttributeComp = Cast<UYAttributeComponent>(InstigatorPawn->GetComponentByClass(UYAttributeComponent::StaticClass()));

	if (AttributeComp && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditsCost);
}


#undef LOCTEXT_NAMESPACE