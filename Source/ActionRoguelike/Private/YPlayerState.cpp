// Fill out your copyright notice in the Description page of Project Settings.


#include "YPlayerState.h"
#include <YSaveGame.h>
#include <Net/UnrealNetwork.h>





void AYPlayerState::AddCredits(int32 Delta)
{
	if (!ensure(Delta > 0.0f))
	{
		return;
	}

	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}


bool AYPlayerState::RemoveCredits(int32 Delta)
{
	if (!ensure(Delta > 0.0f))
	{
		return false;
	}

	if (Credits < Delta)
	{
		return false;
	}

	Credits -= Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);

	return true;
}


void AYPlayerState::SavePlayerState_Implementation(UYSaveGame* SaveObject)
{
	if (SaveObject)
	{
		SaveObject->Credits = Credits;
	}
}


void AYPlayerState::LoadPlayerState_Implementation(UYSaveGame* SaveObject)
{
	if (SaveObject)
	{
		AddCredits(SaveObject->Credits - Credits);
	}
}


void AYPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}


int32 AYPlayerState::GetCredits() const
{
	return Credits;
}


void AYPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AYPlayerState, Credits);
}