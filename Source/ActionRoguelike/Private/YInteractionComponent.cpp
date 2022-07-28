// Fill out your copyright notice in the Description page of Project Settings.


#include "YInteractionComponent.h"
#include "YGameplayInterface.h"
#include "DrawDebugHelpers.h"
#include "YCharacter.h"
#include "Camera/CameraComponent.h"
#include "YWorldUserWidget.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, 
	TEXT("Enalbe Debug Ling for Interact Component."), ECVF_Cheat);


UYInteractionComponent::UYInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistance = 500.0f;
	TraceRadius = 30.0f;

	CollisionChannel = ECC_WorldDynamic;
}


void UYInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UYInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	APawn* MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
		/*GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, GetNameSafe(FcousedActor));*/
	}
}


void UYInteractionComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	TArray<FHitResult> Hits;

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	FcousedActor = nullptr;
	for (FHitResult Hit : Hits)
	{
		if (bDebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 0.0f);
		}

		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<UYGameplayInterface>())
			{
				FcousedActor = HitActor;
				break;
			}
		}
	}

	if (FcousedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<UYWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		} 

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FcousedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 0.0f);
	}
}


void UYInteractionComponent::PrimaryInteract()
{
	ServerInteract(FcousedActor);
}


void UYInteractionComponent::ServerInteract_Implementation(AActor* InFcous)
{
	if (InFcous == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());
	IYGameplayInterface::Execute_Interact(InFcous, MyPawn);
}