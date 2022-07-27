// Fill out your copyright notice in the Description page of Project Settings.


#include "YPowerUp_HealthPotion.h"
#include "YAttributeComponent.h"
#include "YPlayerState.h"





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
