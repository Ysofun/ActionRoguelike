// Fill out your copyright notice in the Description page of Project Settings.


#include "YMagicProjectile.h"
#include "YAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "YGameplayFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "YActionComponent.h"
#include "YActionEffect.h"



// Sets default values
AYMagicProjectile::AYMagicProjectile()
{
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AYMagicProjectile::OnActorOverlap);

	RageAmount = 20.0f;
	DamageAmount = 20.0f;
}

void AYMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ensure(OtherActor) && OtherActor != GetInstigator())
	{
		UYActionComponent* ActionComp = Cast<UYActionComponent>(OtherActor->GetComponentByClass(UYActionComponent::StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = -MovementComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		if (UYGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();

			if (ActionComp)
			{
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}

		UYGameplayFunctionLibrary::ApplyRage(GetInstigator(), OtherActor, RageAmount);
	}
}