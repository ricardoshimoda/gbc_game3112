// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TPSWeapon.h"
#include "TPSCharacter.generated.h"


//ADDITIONS: forward declarations
class UCameraComponent;
class USpringArmComponent;

class ATPSWeapon;
class UBoxComponent;

UCLASS()
class LAB1_API ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//ADDITIONS
	void MoveForward(float val);
	void MoveRight(float val);
	void BeginCrouch();
	void EndCrouch();
	void Jump();
	
	//It's a macro. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerProperties")
	bool bIsAiming;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerProperties", meta = (ClampMin = 30, ClampMax = 120))
	float defaultFoV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerProperties", meta = (ClampMin = 30, ClampMax = 120))
	float zoomedFoV;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerProperties")
	bool bInCover;
	float dt;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	TSubclassOf<ATPSWeapon> StarterWeaponClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponProperties")
	FName WeaponSocketName;
	ATPSWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerProperties")
	UBoxComponent* overlappingCoverVolume;

	bool FinalWeaponFireMode;

	void StartZoom();
	void EndZoom();
	void FireWeapon();
	void TakeCover();
	void StartFire();
	void StopFire();
	void ChangeWeaponMode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector GetPawnViewLocation() const final override;
};
