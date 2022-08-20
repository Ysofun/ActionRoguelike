// Fill out your copyright notice in the Description page of Project Settings.


#include "YPowerUpActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"





// Sets default values
AYPowerUpActor::AYPowerUpActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("PowerUp");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	RespawnTime = 10.0f;

	SetReplicates(true);
	SphereComp->SetIsReplicated(true);
	MeshComp->SetIsReplicated(true);
}

void AYPowerUpActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// Interact logic
}

FText AYPowerUpActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

void AYPowerUpActor::ShowPowerUp()
{
	SetPowerUpState(true);
}

void AYPowerUpActor::HideAndCooldownPowerUp()
{
	SetPowerUpState(false);
	
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AYPowerUpActor::ShowPowerUp, RespawnTime);
}

void AYPowerUpActor::SetPowerUpState(bool bNewIsActive)
{
	SetActorEnableCollision(bNewIsActive);

	RootComponent->SetVisibility(bNewIsActive, true);
}
