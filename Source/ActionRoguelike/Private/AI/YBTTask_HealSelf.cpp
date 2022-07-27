// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/YBTTask_HealSelf.h"
#include "YAttributeComponent.h"
#include "GameFramework/Character.h"
#include "AIController.h"

EBTNodeResult::Type UYBTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyCharacter = Cast<ACharacter>(MyController->GetPawn());
		if (MyCharacter == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		UYAttributeComponent* AttributeComp = UYAttributeComponent::GetAttribute(MyCharacter);
		if (AttributeComp == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		AttributeComp->ApplyHealthChange(MyCharacter, AttributeComp->GetHealthMax());
	}
	return EBTNodeResult::Failed;
}
