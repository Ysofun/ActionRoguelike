// Fill out your copyright notice in the Description page of Project Settings.


#include "YPlayerController.h"



void AYPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}

void AYPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}

void AYPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReceived.Broadcast(PlayerState);
}
