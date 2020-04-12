// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"


/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API ATPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_WaveStarter;
	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	float timeBetweenWaves;
	int waveNumber = 0;
	int numberOfBotsToSpawn;
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Mode")
	void SpawnNewBot();
	void SpawnBotTimerElapsed();
	void StartWave();
	void EndWave();
	void PrepareForNextWave();
	void CheckWaveState();

	// Lobby
	void PlayerJoined_Implementation();
	bool PlayerJoined_Validate();

public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	ATPSGameMode();
	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;
	int PlayerNumber = 0;
};
