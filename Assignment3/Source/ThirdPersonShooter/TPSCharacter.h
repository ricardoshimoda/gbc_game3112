// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

UENUM()		
enum class WeaponState : uint8
{
	Idle 			UMETA(DisplayName = "Idle"),
	Shooting 		UMETA(DisplayName = "Shooting"),
	Reloading		UMETA(DisplayName = "Reloading"),
	Switching		UMETA(DisplayName = "Switching")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, ATPSCharacter*, actor);

class ATPSWeapon;
class ATPSPowerUpInvincibility;
class ATPSPowerUpMegaDamage;
class UBoxComponent;
class UHealthComponent;
class UStaticMeshComponent;
class UMaterial;
UCLASS()
class THIRDPERSONSHOOTER_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float val);
	void MoveSideways(float val);
	void BeginCrouch();
	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerProperties")
	bool bIsAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	TArray<TSubclassOf<ATPSWeapon>> StarterWeaponClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	FName HandSocketName;
	ATPSWeapon* CurrentWeapon;
	
	TArray<ATPSWeapon*> Weapons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	TArray<FName> WeaponSlotSocketNames;

	int currentWeaponSlot;
	
	UFUNCTION(BlueprintCallable)
	void EquipWeaponAtCurrentSlot();
	
	void EquipWeaponAtSlot(int slot);
	
	UFUNCTION(BlueprintCallable)
	void FinishSwitching();
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponProperties")
	TEnumAsByte<WeaponState> currentWeaponState = WeaponState::Idle;

	void HandIK();

	void WeaponPickUpControl(FVector &EyeLoc, FRotator &EyeRot);

	void InvincibilityPickUpControl(FVector &EyeLoc, FRotator &EyeRot);
	void DamagerPickUpControl(FVector &EyeLoc, FRotator &EyeRot);
public:
	inline WeaponState GetCurrentWeaponState() { return currentWeaponState; }
protected:
	UPROPERTY(BlueprintReadWrite, Category = "WeaponProperties")
	bool bPlaySwitchAnim;
	void NextWeapon();
	void PreviousWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CoverProperties")
	UBoxComponent* overlappingCoverVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CoverProperties")
	bool bInCover;
	float dt;

	UFUNCTION(BlueprintCallable)
	virtual void StartZoom();
	UFUNCTION(BlueprintCallable)
	virtual void EndZoom();
	void FireWeapon();
	UFUNCTION(BlueprintCallable)
	void StartFire();
	UFUNCTION(BlueprintCallable)
	void EndFire();
	UFUNCTION(BlueprintCallable)
	void TakeCover();

	// Health Params
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComp;
	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerProperties")
	bool bDead;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerProperties")
	bool destroyOnDeath = true;
	FTimerHandle WeaponDetachTimer;
	void DetatchWeapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerProperties")
	UMaterialInterface* deathMaterial;
	UPROPERTY(BlueprintReadOnly, Category = "PlayerProperties")
	bool bPlayReloadAnimFlag;

	// IK
	UPROPERTY(BlueprintReadOnly, Category = "IK Properties")
	FVector LeftHandIKLocation;
	UPROPERTY(BlueprintReadOnly, Category = "IK Properties")
	FRotator LeftHandIKRotation;

	// Pickup
	ATPSWeapon* pickableWeapon;
	TArray<AActor*> actorsToIgnoreForPickup;
	void RefreshPickupIgnores();
	void PickUpWeapon();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Properties")
	FVector pickupBoxHalfSize = FVector(100, 100, 200);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Properties")
	float pickupDistance = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Properties")
	float invincibilityDuration = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Properties")
	float damagerDuration = 10;

	ATPSPowerUpInvincibility* pickableInvincible;
	ATPSPowerUpMegaDamage* pickableMegaDamage;
	FTimerHandle InvincibleTimer;
	FTimerHandle DamageTimer;
	void PickUpInvincible();
	void PickUpDamager();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blink")
	UMaterialInterface* OriginalMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blink")
	UMaterialInterface* BlinkMaterial;

public:	
	UFUNCTION(BlueprintCallable)
	void PlayReloadAnim();
	UFUNCTION(BlueprintCallable)
	void ReloadAnimStarted();
	UFUNCTION(BlueprintCallable)
	void FinishReload();
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FVector GetPawnViewLocation() const override;
	void FadeOutDamager();
	void FadeOutInvincibility();
};
