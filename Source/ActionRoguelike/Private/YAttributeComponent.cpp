// Fill out your copyright notice in the Description page of Project Settings.


#include "YAttributeComponent.h"
#include "YGameModeBase.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);


UYAttributeComponent::UYAttributeComponent()
{
	HealthMax = 100;
	Health = HealthMax;

	RageMax = 100;
	Rage = 0;

	SetIsReplicatedByDefault(true);
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
	float NewHealth = FMath::Clamp<float>(Health + Delta, 0, HealthMax);

	float ActualDelta = NewHealth - OldHealth;
	
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (ActualDelta != 0.0f)
		{
			MulticastHealthChanged(InstigatorActor, Health, Delta);
		}

		if (ActualDelta < 0.0f && Health == 0.0f)
		{
			AYGameModeBase* GM = GetWorld()->GetAuthGameMode<AYGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return ActualDelta != 0;
}

bool UYAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	float OldRage = Rage;
	float NewRage = FMath::Clamp<float>(Rage + Delta, 0, RageMax);

	float ActualDelta = Rage - OldRage;

	if (GetOwner()->HasAuthority())
	{
		Rage = NewRage;

		if (ActualDelta != 0)
		{
			MulticastRageChanged(InstigatorActor, Rage, Delta);
		}
	}

	return ActualDelta != 0;
}


void UYAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}


void UYAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}

bool UYAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
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


void UYAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UYAttributeComponent, Health);
	DOREPLIFETIME(UYAttributeComponent, HealthMax);
}