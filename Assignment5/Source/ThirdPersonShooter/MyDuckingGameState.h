// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyDuckingGameState.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSONSHOOTER_API AMyDuckingGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "PlayerProperties")
	int playerNumber = 0;
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void PlayerJoined();
	void PlayerJoined_Implementation();
	bool PlayerJoined_Validate();
	AMyDuckingGameState();
};
