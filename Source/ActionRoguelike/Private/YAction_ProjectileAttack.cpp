// Fill out your copyright notice in the Description page of Project Settings.


#include "YAction_ProjectileAttack.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"





UYAction_ProjectileAttack::UYAction_ProjectileAttack()
{
	AttackAnimDelay = 0.2f;
	HandSocketName = "Muzzle_01";
}


void UYAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);
		UGameplayStatics::SpawnEmitterAttached(AttachEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, 
			FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

			GetWorldTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
	
}


void UYAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		// 获取相机位置与控制器角度，然后发射一条射线
		FHitResult Hit;

		float Radius = 20.0f;
		FCollisionShape Shape;
		Shape.SetSphere(Radius);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(InstigatorCharacter);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();
		FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000);

		bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, QueryParams);

		// 获取手部发射弹药的位置
		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);
		// 计算发射方向
		FRotator AimRotation;
		if (bBlockingHit)
		{
			TraceEnd = Hit.ImpactPoint;
		}

		FColor SphereColor = bBlockingHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, SphereColor, false, 2.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), TraceEnd, Radius, 32, SphereColor, false, 2.0f);

		AimRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, TraceEnd);
		FTransform SpawnTM = FTransform(AimRotation, HandLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
		UE_LOG(LogTemp, Log, TEXT("Projectile is %s"), *GetNameSafe(ProjectileClass));
	}

	StopAction(InstigatorCharacter);
}

