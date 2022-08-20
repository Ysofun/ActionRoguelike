// Fill out your copyright notice in the Description page of Project Settings.


#include "YAnimInstance.h"
#include <YActionComponent.h>
#include "GameplayTagContainer.h"




void UYAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* OwningActor = GetOwningActor();
	if (OwningActor)
	{
		ActionComp = Cast<UYActionComponent>(OwningActor->GetComponentByClass(UYActionComponent::StaticClass()));
	}
}

void UYAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");
	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
