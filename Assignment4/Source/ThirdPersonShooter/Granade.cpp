// Fill out your copyright notice in the Description page of Project Settings.


#include "Granade.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h" //apparently, this is required for GameplayStatics as well...
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

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
	PlayExplosionEffect();
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, expBaseDamage, GetActorLocation(), expRadius, nullptr, IgnoreActors,
		this, GetInstigatorController(), true);
	Destroy();

}

void AGranade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGranade, expDelay);
	DOREPLIFETIME(AGranade, expRadius);
	DOREPLIFETIME(AGranade, expBaseDamage);
	DOREPLIFETIME(AGranade, expEffect);
	DOREPLIFETIME(AGranade, expSound);
}

void AGranade::PlayExplosionEffect_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), expEffect, GetActorLocation());
	UGameplayStatics::SpawnSoundAtLocation(this, expSound, GetActorLocation());
}

// Called every frame
void AGranade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

