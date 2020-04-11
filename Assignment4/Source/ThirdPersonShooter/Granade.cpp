// Fill out your copyright notice in the Description page of Project Settings.


#include "Granade.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h" //apparently, this is required for GameplayStatics as well...
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AGranade::AGranade()
{
	bReplicates = true;
	bReplicateMovement = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);

}

// Called when the game starts or when spawned
void AGranade::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(expTimer, this, &AGranade::Explode, 0.001f, false, expDelay);

}

void AGranade::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), expEffect, GetActorLocation());
	UGameplayStatics::SpawnSoundAtLocation(this, expSound, GetActorLocation());
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, expBaseDamage, GetActorLocation(), expRadius, nullptr, IgnoreActors,
		this, GetInstigatorController(), true);
	Destroy();

}

// Called every frame
void AGranade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

