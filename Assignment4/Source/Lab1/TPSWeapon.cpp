// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h" //for damage and particle effects
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Lab1.h"

int32 DebugDrawWeapons = 0;

FAutoConsoleVariableRef CVARDrawWeapons = FAutoConsoleVariableRef(
	TEXT("TPS.DebugDrawWeapons"),
	DebugDrawWeapons,
	TEXT("Draw debug weapon line trace"),
	ECVF_Cheat
);

// Sets default values
ATPSWeapon::ATPSWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	RootComponent = (USceneComponent*)MeshComp;

	TrailEffectParameter = "BeamEnd";
}

// Called when the game starts or when spawned
void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATPSWeapon::Fire() {

	// Return when there are no bullets
	if (AmmoCount <= 0) return;

	AmmoCount--;
	if (CurrentWeaponFireMode == WeaponFireModeEnum::WE_Continuous)
	{
		currentBulletSpread += BASE_BULLET_SPREAD_RATE;
		if (currentBulletSpread >= 1.0f)
			currentBulletSpread = 1.0f;
	}

	//create an owner for the weapon
	//AActor* MyOwner = GetOwner();
	APawn* MyOwner = Cast<APawn>( GetOwner());

	if (MyOwner) {
		//LineTracing requires a range (length)
		FVector EyeLocation;
		FRotator EyeRotation; //FRotators are, basically, quaternions
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		// Now we interfere with the aim
		FVector FinalVector = EyeRotation.Vector();
		FVector PerperndiculatVector = FVector(FinalVector.Y, FinalVector.X, 0);
		FinalVector = (1 - currentBulletSpread) * FinalVector + currentBulletSpread * PerperndiculatVector;
		FVector destination = EyeLocation + 10000 * FinalVector;
		FVector TrailEnd = destination;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = false;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult HitResult;
		
		if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, destination, ECC_Visibility, QueryParams)) {
			
			AActor* victim = HitResult.GetActor();

			EPhysicalSurface  SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Cast<UPhysicalMaterial>(HitResult.PhysMaterial));

			UParticleSystem* ImpactEffectToPlay = NULL;
			float DamageToApply = BaseDamage;

			switch (SurfaceType) {
				case Flesh_Default:
					ImpactEffectToPlay = ImpactEffectBlood;
					break;
				case Flesh_Vulnerable:
					ImpactEffectToPlay = ImpactEffectBlood;
					DamageToApply *= DamageMultiplier;
					break;
				case Concrete:
					ImpactEffectToPlay = ImpactEffectConcrete;
					break;
				default:
					ImpactEffectToPlay = ImpactEffectBlood;
					break;
			}


			UGameplayStatics::ApplyPointDamage(victim, 20.0F, EyeRotation.Vector(), HitResult, MyOwner->GetInstigatorController(), this, damageType);

			TrailEnd = HitResult.ImpactPoint; //gotta read smoke trail end
			if (ImpactEffectToPlay) {
				//what happens when it hits. Different effects go here
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffectToPlay, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}

		}

		FVector MuzzlePosition = MeshComp->GetSocketLocation(MuzzleSocket);

		if (MuzzleEffect) {
			//fire from muzzle
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, (USceneComponent*)MeshComp, MuzzleSocket, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		}

		if (TrailEffect) {
			//smoke trail
			UParticleSystemComponent* TrailComp =  UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailEffect, MuzzlePosition);

			if (TrailComp) {
				TrailComp->SetVectorParameter(TrailEffectParameter, TrailEnd);
			}
		}

		if (DebugDrawWeapons > 0) {
			DrawDebugLine(GetWorld(), EyeLocation, destination, FColor::Cyan, false, 1.0, 0, 2.5f);
		}

		// Camera Shake
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController) {
			PlayerController->ClientPlayCameraShake(FireCameraShake);
		}
	}

	
}

void ATPSWeapon::StartFire() {
	GetWorldTimerManager().SetTimer(BulletTimer, this, &ATPSWeapon::Fire, 0.1f, true, 0.0f );
}

void ATPSWeapon::StopFire() {
	GetWorldTimerManager().ClearTimer(BulletTimer);
	currentBulletSpread = BASE_BULLET_SPREAD;
}

WeaponFireModeEnum ATPSWeapon::ChangeWeaponFireMode()
{
	UE_LOG(LogTemp, Warning, TEXT("Changing Weapon Fire Mode"));
	if (CurrentWeaponFireMode == WeaponFireModeEnum::WE_Single)
	{
		UE_LOG(LogTemp, Warning, TEXT("Changing to continuous"));
		CurrentWeaponFireMode = WeaponFireModeEnum::WE_Continuous;
		return WeaponFireModeEnum::WE_Continuous;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Changing to single"));
		CurrentWeaponFireMode = WeaponFireModeEnum::WE_Single;
		return WeaponFireModeEnum::WE_Single;
	}
	return CurrentWeaponFireMode;
}
