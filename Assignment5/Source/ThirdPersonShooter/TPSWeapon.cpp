// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ThirdPersonShooter.h"
#include "TPSCharacter.h"
#include "TPSPlayer.h"

int32 DebugDrawWeapons = 0;

FAutoConsoleVariableRef CVARDrawWeapons = FAutoConsoleVariableRef(
	TEXT("TPS.DebugDrawWeapons"),
	DebugDrawWeapons,
	TEXT("Draw debug weapon line trace"),
	ECVF_Cheat);

// Sets default values
ATPSWeapon::ATPSWeapon()
{
	bReplicates = true;
	bReplicateMovement = true;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Comp"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	MeshComp->SetupAttachment(RootComponent);
	TrailEffectParam = "BeamEnd";
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	ammoCount = magazineSize;
	//MuzzleSocketName = "Muzzle";
}

void ATPSWeapon::PlayImpactEffect_Implementation(EPhysicalSurface _SurfaceType, FVector _ImpactPoint, FVector _ImpactNormal)
{
	UParticleSystem* ImpactEffectToPlay = NULL;

	switch (_SurfaceType)
	{
	case Flash_Default:
		ImpactEffectToPlay = ImpactEffectBlood;
		break;
	case Flash_Vulnerable:
		ImpactEffectToPlay = ImpactEffectBlood;
		break;
	case Concrete:
		ImpactEffectToPlay = ImpactEffectConcrete;
		break;
	default:
		ImpactEffectToPlay = ImpactEffectConcrete;
		break;
	}

	if (ImpactEffectToPlay)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectToPlay, _ImpactPoint, _ImpactNormal.Rotation());
	}
}

void ATPSWeapon::PlayMuzzleEffect_Implementation()
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
}

void ATPSWeapon::PlayTracerEffect_Implementation(FVector _traceEnd)
{
	FVector MuzzlePosition = MeshComp->GetSocketLocation(MuzzleSocketName);

	if (TrailEffect)
	{
		UParticleSystemComponent* TrailComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailEffect, MuzzlePosition);

		if (TrailComp)
		{
			TrailComp->SetVectorParameter(TrailEffectParam, _traceEnd);
		}
	}

}

void ATPSWeapon::Reload()
{
	if (totalNumberOfBullets > magazineSize)
	{
		ammoCount = magazineSize;
		totalNumberOfBullets -= magazineSize;
	}
	else
	{
		ammoCount = totalNumberOfBullets;
		totalNumberOfBullets = 0;
	}
	ATPSCharacter* MyOwner = Cast<ATPSCharacter>(GetOwner());
	if (MyOwner) {
		MyOwner->TriggerOnWeaponAmmoChange(weaponIcon, ammoCount, magazineSize);
	}
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSWeapon::StartFire()
{
	if (!firing)
	{
		GetWorldTimerManager().SetTimer(BulletTimer, this, &ATPSWeapon::Fire, 1.0f / fireRate, true, 0.0f);
		firing = true;
	}
}

void ATPSWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(BulletTimer);
	firing = false;
}

void ATPSWeapon::Fire()
{
	ATPSCharacter* MyOwner = Cast<ATPSCharacter>(GetOwner());

	if (MyOwner && ammoCount > 0 && MyOwner->GetCurrentWeaponState() == WeaponState::Shooting)
	{
		FVector EyeLoc;
		FRotator EyeRot;
		MyOwner->GetActorEyesViewPoint(EyeLoc, EyeRot);

		FVector LineEnd = EyeLoc + 10000 * EyeRot.Vector();
		FVector TrailEnd = LineEnd;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);

		// Gets every actor of type player
		ATPSPlayer* curPl = Cast<ATPSPlayer>(MyOwner);
		if (curPl) {
			TSubclassOf<ATPSPlayer> classToFind;
			classToFind = ATPSPlayer::StaticClass();
			TArray<AActor*> foundPlayers;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundPlayers);
			for (auto& act : foundPlayers) {
				ATPSPlayer * pl = Cast<ATPSPlayer>(act);
				if (pl && pl->currentPlayerTeam == curPl->currentPlayerTeam) {
					QueryParams.AddIgnoredActor(pl);
				}
			}
		}
		QueryParams.bTraceComplex = false;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLoc, LineEnd, WeaponTraceChannel, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Cast <UPhysicalMaterial>(Hit.PhysMaterial));

			UParticleSystem* ImpactEffectToPlay = NULL;
			float DamageToApply = BaseDamage;
			if (SurfaceType == Flash_Vulnerable)
			{
				DamageToApply *= DamageMultiplier;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, DamageToApply, EyeRot.Vector(), Hit, MyOwner->GetInstigatorController(), this, DamageType);

			TrailEnd = Hit.ImpactPoint;

			PlayImpactEffect(SurfaceType, Hit.ImpactPoint, Hit.ImpactNormal);
		}

		PlayMuzzleEffect();

		PlayTracerEffect(TrailEnd);

		if (DebugDrawWeapons > 0)
		{
			DrawDebugLine(GetWorld(), EyeLoc, LineEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		// Camera shake
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientPlayCameraShake(FireCameraShake);
		}

		ammoCount--;
		MyOwner->TriggerOnWeaponAmmoChange(weaponIcon, ammoCount, magazineSize);
		if (ammoCount == 0 && totalNumberOfBullets > 0)
		{
			MyOwner->PlayReloadAnim();
		}
	}


}

