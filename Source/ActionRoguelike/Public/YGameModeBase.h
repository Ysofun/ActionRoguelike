// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "YGameModeBase.generated.h"


class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class AYPlayerState;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API AYGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category = "AI");
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Powerups")
	int32 CreditsPerKill;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	UEnvQuery* PowerupSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<TSubclassOf<AActor>> PowerupClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 DesiredPowerupCount;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float RequiredPowerupDistance;

	FTimerHandle TimerHandle_SpawnBots;

	UFUNCTION()
	void SpawnBotTimeElapsed();

	UFUNCTION()
	void OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	AYGameModeBase();

	virtual void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();
};
