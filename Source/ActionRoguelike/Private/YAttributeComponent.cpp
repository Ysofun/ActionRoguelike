// Fill out your copyright notice in the Description page of Project Settings.


#include "YAttributeComponent.h"
#include "YGameModeBase.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


UYAttributeComponent* UYAttributeComponent::GetAttribute(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<UYAttributeComponent>(FromActor->GetComponentByClass(UYAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool UYAttributeComponent::IsActorAlive(AActor* Actor)
{
	UYAttributeComponent* AttributeComp = GetAttribute(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

// Sets default values for this component's properties
UYAttributeComponent::UYAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;
}

bool UYAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

bool UYAttributeComponent::IsFullHealth() const
{
	return Health == HealthMax;
}

bool UYAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

float UYAttributeComponent::GetHealth() const
{
	return Health;
}

float UYAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

bool UYAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	float OldHealth = Health;

	Health = FMath::Clamp<float>(Health + Delta, 0, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		AYGameModeBase* GM = GetWorld()->GetAuthGameMode<AYGameModeBase>();
		if (GM)
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0;
}
