// Fill out your copyright notice in the Description page of Project Settings.


#include "YPlayerController.h"



void AYPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}
