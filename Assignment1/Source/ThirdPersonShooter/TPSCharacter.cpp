// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TPSWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "HealthComponent.h"
#include "ThirdPersonShooter.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ATPSCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	EndZoom();	

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<ATPSWeapon>(StarterWeaponClass, spawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(Cast<USceneComponent>(GetMesh()), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			WeaponSocketName);
	}
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	dt = DeltaTime;
	if (bInCover)
	{
		if (bIsAiming)
		{
			GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		}
		else
		{
			if (overlappingCoverVolume)
			{
				GetMesh()->SetWorldRotation(overlappingCoverVolume->GetComponentRotation().Add(0, 40, 0));
			}
		}
	}
	else
	{
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	}
}

void ATPSCharacter::MoveForward(float val)
{
	if (!bInCover)
	{
		GetCharacterMovement()->AddInputVector(GetActorForwardVector() * val);
	}
}

void ATPSCharacter::MoveSideways(float val)
{
	if (bInCover)
	{
		FVector targetPos = GetActorLocation() + overlappingCoverVolume->GetRightVector() * val * dt;
		FVector offsetFromCoverCenter = targetPos - overlappingCoverVolume->GetComponentLocation();
		float dot = FVector::DotProduct(offsetFromCoverCenter, overlappingCoverVolume->GetRightVector());
		bool isTargetInRange = abs(dot) < overlappingCoverVolume->GetScaledBoxExtent().Y - 50;
		bool isMovingTowardsCenter = dot * val < 0;
		if (isTargetInRange || isMovingTowardsCenter)
		{
			GetCharacterMovement()->AddInputVector(overlappingCoverVolume->GetRightVector() * val);
		}
	}
	else
	{
		GetCharacterMovement()->AddInputVector(GetActorRightVector() * val);
	}
}

void ATPSCharacter::BeginCrouch()
{
	Crouch();
}

void ATPSCharacter::EndCrouch()
{
	UnCrouch();
}

void ATPSCharacter::StartZoom()
{
	bIsAiming = true;
}

void ATPSCharacter::EndZoom()
{
	bIsAiming = false;
}

void ATPSCharacter::FireWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}

void ATPSCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ATPSCharacter::EndFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
	}
}

void ATPSCharacter::TakeCover()
{
	if (bInCover)
	{
		bInCover = false;
	}
	else if (overlappingCoverVolume && !bInCover)
	{
		FVector lineTraceStart = GetActorLocation();
		FVector lineTraceEnd = GetActorLocation() + overlappingCoverVolume->GetForwardVector() * 10000;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, lineTraceStart, lineTraceEnd, CoverTraceChannel))
		{
			FVector targetLocation = Hit.Location;
			targetLocation -= overlappingCoverVolume->GetForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
			DrawDebugSphere(GetWorld(), targetLocation, 10, 24, FColor::Yellow, false, 5, 0, 2);
			SetActorLocation(targetLocation);

			bInCover = true;
		}
	}
}

void ATPSCharacter::DetatchWeapon()
{
	CurrentWeapon->MeshComp->SetSimulatePhysics(true);
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}
void ATPSCharacter::OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0)
	{
		bDead = true;
		OnDeath.Broadcast(this);
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		if (destroyOnDeath)
		{
			SetLifeSpan(5);
		}
		GetWorldTimerManager().SetTimer(WeaponDetachTimer, this, &ATPSCharacter::DetatchWeapon, 3, false, 0.2f);
		GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, deathMaterial);
		GetMesh()->SetScalarParameterValueOnMaterials("StartTime", UGameplayStatics::GetRealTimeSeconds(this));
	}
}


// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector ATPSCharacter::GetPawnViewLocation() const
{
	return Super::GetPawnViewLocation();
}

