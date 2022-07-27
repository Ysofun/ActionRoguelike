// Fill out your copyright notice in the Description page of Project Settings.


#include "YPowerUp_Credits.h"
#include "YPlayerState.h"





AYPowerUp_Credits::AYPowerUp_Credits()
{
	CreditsAmount = 80;
}


void AYPowerUp_Credits::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	if (AYPlayerState* PS = InstigatorPawn->GetPlayerState<AYPlayerState>())
	{
		PS->AddCredits(CreditsAmount);
		HideAndCooldownPowerUp();
	}
}