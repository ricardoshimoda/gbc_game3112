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
	Switching		UMETA(DisplayName = "Switching"),
	PickingUp       UMETA(DisplayName = "PickingUp")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, ATPSCharacter*, actor);

class ATPSWeapon;
class UBoxComponent;
class UHealthComponent;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponSwitchSignature, ATPSWeapon*, currentWeapon, ATPSWeapon*, previousWeapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponAmmoChangeSignature, UTexture2D*, weaponTexture, float, ammoCount, float, maxAmmo);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "PlayerProperties")
	bool bIsAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	TArray<TSubclassOf<ATPSWeapon>> StarterWeaponClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	FName HandSocketName;

	UPROPERTY(Replicated)
	ATPSWeapon* CurrentWeapon;
	bool  isCurrentWeaponLoadedOnClient = false;

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
public:
	inline WeaponState GetCurrentWeaponState() { return currentWeaponState; }
protected:
	UPROPERTY(BlueprintReadWrite, Category = "WeaponProperties")
	bool bPlaySwitchAnim;
	UFUNCTION(NetMulticast, Unreliable)
	void TriggerSwitchAnim();
	UFUNCTION(Server, Reliable, WithValidation)
	void NextWeapon();
	void NextWeapon_Implementation();
	bool NextWeapon_Validate();
	UFUNCTION(Server, Reliable, WithValidation)
	void PreviousWeapon();
	void PreviousWeapon_Implementation();
	bool PreviousWeapon_Validate();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CoverProperties")
	UBoxComponent* overlappingCoverVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "CoverProperties")
	bool bInCover;
	float dt;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void StartZoom();
	virtual void StartZoom_Implementation();
	bool StartZoom_Validate();
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void EndZoom();
	virtual void EndZoom_Implementation();
	bool EndZoom_Validate();
	void FireWeapon();
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void StartFire();
	virtual void StartFire_Implementation();
	bool StartFire_Validate();
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void EndFire();
	virtual void EndFire_Implementation();
	bool EndFire_Validate();
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void TakeCover();
	void TakeCover_Implementation();
	bool TakeCover_Validate();

	// Health Params
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComp;
	UFUNCTION()
	void OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated,Category = "PlayerProperties")
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
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "IK Properties")
	FVector LeftHandIKLocation;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "IK Properties")
	FRotator LeftHandIKRotation;
	UPROPERTY(BlueprintReadOnly, Category = "IK Properties")
	FVector leftFootIkPos;
	UPROPERTY(BlueprintReadOnly, Category = "IK Properties")
	FVector rightFootIkPos;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK Properties")
	float ikDistance = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IK Properties")
	float feetOffest = 12;
	UPROPERTY(BlueprintReadOnly, Category = "IK Properties")
	bool applyIk;
	FVector originalMeshLocation;

	// Pickup
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Pickup Properties")
	ATPSWeapon* pickableWeapon;
	TArray<AActor*> actorsToIgnoreForPickup;
	void RefreshPickupIgnores();
	void PickUpWeapon();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Properties")
	FVector pickupBoxHalfSize = FVector(100, 100, 200);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Properties")
	float pickupDistance = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup Properties")
	float pickupTime = 1.0f;
	FTimerHandle pickupTimer;
	UFUNCTION(Server, Reliable, WithValidation)
	void StartPickup();
	void StartPickup_Implementation();
	bool StartPickup_Validate();
	UFUNCTION(NetMulticast, Reliable)
	void StartPickupTimer();
	UFUNCTION(Server, Reliable, WithValidation)
	void CancelPickup();
	void CancelPickup_Implementation();
	bool CancelPickup_Validate();
	UFUNCTION(NetMulticast, Reliable)
	void StopPickupTimer();
	UFUNCTION(BlueprintCallable)
	float GetPickupAlpha();

	// Controller
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Controller Properties")
	float lookYaw;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Controller Properties")
	float lookPitch;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
public:	
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void PlayReloadAnim();
	void PlayReloadAnim_Implementation();
	bool PlayReloadAnim_Validate();
	UFUNCTION(NetMulticast, Unreliable)
	void TriggerReloadAnimFlag();
	UFUNCTION(BlueprintCallable)
	void ReloadAnimStarted();
	UFUNCTION(BlueprintCallable)
	void FinishReload();
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponSwitchSignature OnWeaponSwitch;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnWeaponAmmoChangeSignature OnWeaponAmmoChange;
	/*
	
	*/
	UFUNCTION(NetMulticast, Reliable)
	void TriggerOnWeaponAmmoChange(UTexture2D* weaponTexture, float currentAmmo, float maxAmmo);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FVector GetPawnViewLocation() const override;
};
