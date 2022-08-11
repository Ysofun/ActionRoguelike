// Fill out your copyright notice in the Description page of Project Settings.


#include "YCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "YInteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "YAttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "YActionComponent.h"
#include <YGameplayFunctionLibrary.h>



// Sets default values
AYCharacter::AYCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArm
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	// Camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	// Interact
	InteractionComp = CreateDefaultSubobject<UYInteractionComponent>("InteractionComp");

	// Attribute
	AttributeComp = CreateDefaultSubobject<UYAttributeComponent>("AttributeComp");

	// Action
	ActionComp = CreateDefaultSubobject<UYActionComponent>("ActionComp");

	// Movement Initialize
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Data Initialize
	TimeToHitParamName = "TimeToHit";
	RageAmount = 5.0f;
}

void AYCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttributeComp->OnHealthChanged.AddDynamic(this, &AYCharacter::OnHealthChanged);
}

FVector AYCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

// Called to bind functionality to input
void AYCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AYCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AYCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AYCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SpecialAttack", IE_Pressed, this, &AYCharacter::SpecialAttack);
	PlayerInputComponent->BindAction("DashAttack", IE_Pressed, this, &AYCharacter::DashAttack);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AYCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AYCharacter::SprintStop);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &AYCharacter::PrimaryInteract);
}


void AYCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;

	AddMovementInput(ControlRot.Vector(), value);
}

void AYCharacter::MoveRight(float value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, value);
}

void AYCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void AYCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

#pragma region Attack

void AYCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void AYCharacter::SpecialAttack()
{
	ActionComp->StartActionByName(this, "BlackHole");
}

void AYCharacter::DashAttack()
{
	ActionComp->StartActionByName(this, "Dash");
}

#pragma endregion

void AYCharacter::PrimaryInteract()
{
	if (ensure(InteractionComp))
	{
		InteractionComp->PrimaryInteract();
	}
}


void AYCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}


void AYCharacter::OnHealthChanged(AActor* InstigatorActor, UYAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		UYGameplayFunctionLibrary::ApplyRage(InstigatorActor, this, FMath::Abs(RageAmount * Delta));
	}

	if (NewHealth <= 0.0 && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		SetLifeSpan(5.0f);
	}
}
