// Fill out your copyright notice in the Description page of Project Settings.


#include "YAction.h"
#include "Engine/World.h"
#include "YActionComponent.h"
#include "../ActionRoguelike.h"
#include "Net/UnrealNetwork.h"



void UYAction::Initialize(UYActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}


bool UYAction::CanStart_Implementation(AActor* Instigator)
{
	if (IsRunning())
	{
		return false;
	}

	UYActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;

}

void UYAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	UYActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;
}


void UYAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	//ensureAlways(bIsRunning);

	UYActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);
	
	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;
}


UWorld* UYAction::GetWorld() const
{
	AActor* MyActor = Cast<AActor>(GetOuter());
	if (MyActor)
	{
		return MyActor->GetWorld();
	}
	return nullptr;
}


UYActionComponent* UYAction::GetOwningComponent() const
{
	//return Cast<UYActionComponent>(GetOuter());

	return ActionComp;
}


void UYAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}


bool UYAction::IsRunning() const
{
	return RepData.bIsRunning;
}


void UYAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UYAction, RepData);
	DOREPLIFETIME(UYAction, ActionComp);
}