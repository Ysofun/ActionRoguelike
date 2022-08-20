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
#include "Kismet/GameplayStatics.h"
#include "YSaveGame.h"
#include "GameFramework/GameStateBase.h"
#include <YGameplayInterface.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include "YMonsterData.h"
#include "../ActionRoguelike.h"
#include <YActionComponent.h>
#include <Engine/AssetManager.h>



static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), false, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);


AYGameModeBase::AYGameModeBase()
{
	SpawnTimerInterval = 2.0f;

	CreditsPerKill = 20;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000.0f;

	PlayerStateClass = AYPlayerState::StaticClass();

	SlotName = "SaveGame01";
}


void AYGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
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


void AYGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	AYPlayerState* PS = NewPlayer->GetPlayerState<AYPlayerState>();
	if (PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
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

		if (MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)
			{
				LogOnScreen(this, "Loading monster...", FColor::Green);

				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &AYGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}
	}
}


void AYGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogOnScreen(this, "Finished loading...", FColor::Green);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		UYMonsterData* MonsterData = Cast<UYMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				UYActionComponent* ActionComp = Cast<UYActionComponent>(NewBot->GetComponentByClass(UYActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<UYAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
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
	if (KillerPawn && KillerPawn != VictimActor)
	{
		AYPlayerState* PS = KillerPawn->GetPlayerState<AYPlayerState>();
		if (PS)
		{
			PS->AddCredits(CreditsPerKill);
		}
	}
}


void AYGameModeBase::WriteSaveGame()
{
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		AYPlayerState* PS = Cast<AYPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player at this point
		}
	}

	// clear the actor data array
	CurrentSaveGame->SaveActors.Empty();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		if (!Actor->Implements<UYGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		// Pass the array to fill with data from Actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Actor->Serialize(Ar);


		CurrentSaveGame->SaveActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}


void AYGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<UYSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;

			if (!Actor->Implements<UYGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SaveActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					// Converts binary array back into Actor variables
					Actor->Serialize(Ar);

					IYGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}
		}
	}
	else
	{
		CurrentSaveGame = Cast<UYSaveGame>(UGameplayStatics::CreateSaveGameObject(UYSaveGame::StaticClass()));
	}
}
