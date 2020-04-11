// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"

UENUM()
enum class PlayerTeam : uint8
{
	TeamA 		UMETA(DisplayName = "TeamA"),
	TeamB 		UMETA(DisplayName = "TeamB")
};

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
	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	TArray<TEnumAsByte<PlayerTeam>> playerTeams;
	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	TArray<FName> playerNames;
	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	TArray<FName> teamANames;
	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	TArray<FName> teamBNames;
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void PlayerJoined(int playerIndex, FName playerName);
	int numPlayersA = 0;
	int numPlayersB = 0;

public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	ATPSGameMode();
	virtual void StartPlay() override;
	virtual void Tick(float DeltaTime) override;
};
