// Fill out your copyright notice in the Description page of Project Settings.


#include "YActionEffect.h"
#include "YActionComponent.h"
#include <GameFramework/GameStateBase.h>



UYActionEffect::UYActionEffect()
{
	bAutoStart = true;
}


void UYActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorldTimerManager().SetTimer(TimerHandle_Duration, Delegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodEffect", Instigator);

		GetWorldTimerManager().SetTimer(TimerHandle_Period, Delegate, Period, true);
	}
}


void UYActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if (GetWorldTimerManager().GetTimerRemaining(TimerHandle_Period) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}
	
	Super::StopAction_Implementation(Instigator);

	GetWorldTimerManager().ClearTimer(TimerHandle_Period);
	GetWorldTimerManager().ClearTimer(TimerHandle_Duration);

	UYActionComponent* Comp = GetOwningComponent();
	if (Comp)
	{
		Comp->RemoveAction(this);
	}
}


float UYActionEffect::GetTimeRemaining() const
{
	AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>();
	if (GS)
	{
		float EndTime = TimeStarted + Duration;
		return EndTime - GS->GetServerWorldTimeSeconds();
	}

	return Duration;
}


void UYActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{

}

