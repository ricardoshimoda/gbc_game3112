// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSWeapon.h"
#include "TPSGranadeLauncher.generated.h"

class AGranade;

UCLASS()
class THIRDPERSONSHOOTER_API ATPSGranadeLauncher : public ATPSWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSGranadeLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Fire(); //not sure whether to call override here...

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	TSubclassOf<AGranade> Projectile;
	AGranade* Granade;

	UPROPERTY(EditDefaultsOnly, Category = "Launch Force")
	float LaunchForce;

	UFUNCTION(NetMulticast, Unreliable)
	void PlayTrailEffect(FVector _muzzlePosition, FVector _trailEnd);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
