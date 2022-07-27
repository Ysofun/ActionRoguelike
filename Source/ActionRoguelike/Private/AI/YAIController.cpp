// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/YAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


void AYAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviourTree, TEXT("Behavior Tree is nullptr! Please assign a BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviourTree);
	}

	//APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//if (ensure(MyPawn))
	//{
	//	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", MyPawn->GetActorLocation());

	//	GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
	//}
}
