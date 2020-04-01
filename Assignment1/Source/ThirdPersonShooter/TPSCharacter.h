// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, ATPSCharacter*, actor);

class ATPSWeapon;
class UBoxComponent;
class UHealthComponent;
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
	TSubclassOf<ATPSWeapon> StarterWeaponClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	FName WeaponSocketName;
	ATPSWeapon* CurrentWeapon;
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
public:	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FVector GetPawnViewLocation() const override;
};
