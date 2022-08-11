// Fill out your copyright notice in the Description page of Project Settings.


#include "YActionEffect_Thorns.h"
#include "YAttributeComponent.h"
#include "YActionComponent.h"
#include <YGameplayFunctionLibrary.h>



UYActionEffect_Thorns::UYActionEffect_Thorns()
{
	ReflectFraction = 0.2f;

	Duration = 0.0f;
	Period = 0.0f;
}


void UYActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	UYAttributeComponent* AttributeComp = UYAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());
	if (AttributeComp)
	{
		AttributeComp->OnHealthChanged.AddDynamic(this, &UYActionEffect_Thorns::OnHealthChanged);
	}
}


void UYActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	UYAttributeComponent* AttributeComp = UYAttributeComponent::GetAttribute(GetOwningComponent()->GetOwner());
	if (AttributeComp)
	{
		AttributeComp->OnHealthChanged.RemoveDynamic(this, &UYActionEffect_Thorns::OnHealthChanged);
	}
}


void UYActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, UYAttributeComponent* OwningComp, float NewValue, float Delta)
{
	AActor* OwningActor = GetOwningComponent()->GetOwner();

	if (Delta < 0.0f && OwningActor != InstigatorActor)
	{
		int32 ReflectedAmount = FMath::RoundToInt(Delta * ReflectFraction);
		if (ReflectedAmount == 0)
		{
			return;
		}

		ReflectedAmount = FMath::Abs(ReflectedAmount);

		UYGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, ReflectedAmount);
	}
}
