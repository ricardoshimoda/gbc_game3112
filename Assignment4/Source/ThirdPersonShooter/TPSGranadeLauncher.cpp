// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSGranadeLauncher.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Granade.h"

// Sets default values
ATPSGranadeLauncher::ATPSGranadeLauncher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = (USceneComponent*)MeshComp;

}

// Called when the game starts or when spawned
void ATPSGranadeLauncher::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATPSGranadeLauncher::Fire()
{

	APawn* MyOwner = Cast<APawn>(GetOwner());

	if (MyOwner) {

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		//100(cm) should be enough
		FVector LineEnd = EyeLocation + 100.0f * EyeRotation.Vector();
		FVector TrailEnd = LineEnd;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.bTraceComplex = false;
		QueryParams.bReturnPhysicalMaterial = false;

		if (MuzzleEffect) {
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}

		FVector MuzzlePosition = MeshComp->GetSocketLocation(MuzzleSocketName);

		if (TrailEffect) {

			UParticleSystemComponent* TrailComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailEffect, MuzzlePosition);

			if (TrailComp) {
				//opted for a single 1m trail
				TrailComp->SetVectorParameter(TrailEffectParam, LineEnd);
			}
		}

		FActorSpawnParameters spawnParameters;
		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		spawnParameters.Instigator = MyOwner;

		//spawn grenade
		Granade = GetWorld()->SpawnActor<AGranade>(Projectile, MuzzlePosition, EyeRotation, spawnParameters);
		Granade->MeshComp->AddImpulse(LaunchForce * EyeRotation.Vector());

		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController) {
			PlayerController->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

// Called every frame
void ATPSGranadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

