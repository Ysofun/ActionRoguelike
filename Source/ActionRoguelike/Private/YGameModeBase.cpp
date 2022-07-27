// Fill out your copyright notice in the Description page of Project Settings.


#include "YGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AI/YAICharacter.h"
#include "YAttributeComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "YCharacter.h"
#include "YPlayerState.h"





static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);


AYGameModeBase::AYGameModeBase()
{
	SpawnTimerInterval = 2.0f;

	CreditsPerKill = 20;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000.0f;

	PlayerStateClass = AYPlayerState::StaticClass();
}


void AYGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &AYGameModeBase::SpawnBotTimeElapsed, SpawnTimerInterval, true);

	if (ensure(PowerupClasses.Num() > 0))
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, 
			EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AYGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
}


void AYGameModeBase::KillAll()
{
	for (TActorIterator<AYAICharacter> It(GetWorld()); It; ++It)
	{
		AYAICharacter* Bot = *It;

		UYAttributeComponent* AttributeComp = UYAttributeComponent::GetAttribute(Bot);
		if (AttributeComp && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void AYGameModeBase::SpawnBotTimeElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}

	int32 NrOfAliveBots = 0;
	for (TActorIterator<AYAICharacter> It(GetWorld()); It; ++It)
	{
		AYAICharacter* Bot = *It;

		UYAttributeComponent* AttributeComp = UYAttributeComponent::GetAttribute(Bot);
		if (AttributeComp && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	float MaxBotsCount = 10.0f;
	if (DifficultyCurve)
	{
		MaxBotsCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NrOfAliveBots >= MaxBotsCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AYGameModeBase::OnBotSpawnQueryCompleted);
	}
}


void AYGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}


void AYGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Powerup Spawn EQS Query Failed!"));
		return;
	}

	// Get Result of EQS Query
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// Keep powerup_class distance between points
	TArray<FVector> UsedLocations;

	// Spawned powerup_class number
	int32 SpawnCounter = 0;
	// Spawn powerup_clas
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// Get random index in locations
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		// Get random location to spawn
		FVector PickedLocation = Locations[RandomLocationIndex];
		// Remove to avoid picking again
		Locations.RemoveAt(RandomLocationIndex);

		// Check if location is valid
		bool bValidLocation = true;
		for (FVector UsedLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - UsedLocation).Size();
			if (DistanceTo < RequiredPowerupDistance)
			{
				bValidLocation = false;
				break;
			}
		}

		// Location is not valid
		if (!bValidLocation)
		{
			continue;
		}

		// Pick random powerup_class to spawn
		int32 RandomPowerupClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> PickedPowerupClass = PowerupClasses[RandomPowerupClassIndex];

		// Spawn powerup_class
		GetWorld()->SpawnActor<AActor>(PickedPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// A location is used & number of class ++
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}

void AYGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}

void AYGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	AYCharacter* Player = Cast<AYCharacter>(VictimActor);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));

	APawn* KillerPawn = Cast<APawn>(Killer);
	if (KillerPawn)
	{
		AYPlayerState* PS = KillerPawn->GetPlayerState<AYPlayerState>();
		if (PS)
		{
			PS->AddCredits(CreditsPerKill);
		}
	}
}

