// Fill out your copyright notice in the Description page of Project Settings.


#include "YPowerUp_Action.h"
#include "YActionComponent.h"
#include "YAction.h"
#include "../ActionRoguelike.h"



void AYPowerUpActor_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn && ActionToGrant))
	{
		return;
	}

	UYActionComponent* ActionComp = Cast<UYActionComponent>(InstigatorPawn->GetComponentByClass(UYActionComponent::StaticClass()));
	if (ActionComp)
	{
		if (ActionComp->GetAction(ActionToGrant))
		{
			FString DebugMsg = FString::Printf(TEXT("Action: '%s' already know."), *GetNameSafe(ActionToGrant));
			LogOnScreen(this, DebugMsg);
			return;
		}

		ActionComp->AddAction(InstigatorPawn, ActionToGrant);
		HideAndCooldownPowerUp();
	}
}
