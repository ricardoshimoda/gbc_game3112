// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSWeapon.h"
#include "TPSProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class LAB1_API ATPSProjectileWeapon : public ATPSWeapon
{
	GENERATED_BODY()

public:
	ATPSProjectileWeapon();

protected:
	
	void BeginPlay() override;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent * MeshComp;
	*/
	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	AActor* Grenade;

	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileWeapon")
	FName MuzzleSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ProjectileWeapon")
	UParticleSystem* MuzzleEffect;
	//*/
private:
	void Tick(float deltaTime) override;
};
