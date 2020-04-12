// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDuckingGameState.h"
#include "Net/UnrealNetwork.h"
/*
void AMyDuckingGameState::PlayerJoined()
{
	playerNumber++;
}
*/
void AMyDuckingGameState::PlayerJoined_Implementation()
{
	if (Role == ROLE_Authority) {
		playerNumber++;
	}
}

bool AMyDuckingGameState::PlayerJoined_Validate() {
	return true;
}

AMyDuckingGameState::AMyDuckingGameState()
{
	bReplicates = true;
}

void AMyDuckingGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyDuckingGameState, playerNumber);
}
