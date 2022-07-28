// Fill out your copyright notice in the Description page of Project Settings.


#include "YAttributeComponent.h"
#include "YGameModeBase.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


UYAttributeComponent::UYAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;

	RageMax = 100;
	Rage = 0;
	RagePerHealth = 5.0f;
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

	if (ActualDelta < 0.0f)
	{
		if (Health == 0.0f)
		{
			AYGameModeBase* GM = GetWorld()->GetAuthGameMode<AYGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
		else
		{
			ApplyRageChange(InstigatorActor, RagePerHealth * abs(ActualDelta));
		}
	}

	return ActualDelta != 0;
}

bool UYAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	float OldRage = Rage;

	Rage = FMath::Clamp<float>(Rage + Delta, 0, RageMax);

	float ActualDelta = Rage - OldRage;
	OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);

	return ActualDelta != 0;
}


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

float UYAttributeComponent::GetRage() const
{
	return Rage;
}

float UYAttributeComponent::GetRageMax() const
{
	return RageMax;
}
