// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/YBTService_CheckHealth.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "YAttributeComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"



UYBTService_CheckHealth::UYBTService_CheckHealth()
{
	LowHealthFraction = 0.3f;
}

void UYBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ensure(MyPawn))
	{
		UYAttributeComponent* AttributeComp = UYAttributeComponent::GetAttribute(MyPawn);
		if (ensure(AttributeComp))
		{
			bool bLowHealth = (AttributeComp->GetHealth() / AttributeComp->GetHealthMax()) < LowHealthFraction;

			UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
			BlackBoardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
		}
	}
}
