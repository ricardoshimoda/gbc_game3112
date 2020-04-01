// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSProjectileWeapon.h"

ATPSProjectileWeapon::ATPSProjectileWeapon() {
	PrimaryActorTick.bCanEverTick = true;

	//MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	RootComponent = (USceneComponent*)MeshComp;
}

void ATPSProjectileWeapon::BeginPlay() {
	Super::BeginPlay();
}

void ATPSProjectileWeapon::Fire() {

}

void ATPSProjectileWeapon::Tick(float deltaSeconds) {
	Super::Tick(deltaSeconds);
}