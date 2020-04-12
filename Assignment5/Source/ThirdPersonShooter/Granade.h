// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Granade.generated.h"

class USoundCue;
UCLASS()
class THIRDPERSONSHOOTER_API AGranade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGranade();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Granade Appearance")
		UStaticMeshComponent* MeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Explosion Properties")
	float expDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Explosion Properties")
	float expRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Explosion Properties")
	float expBaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Explosion Properties")
	UParticleSystem* expEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Explosion Properties")
	USoundCue* expSound;

	FTimerHandle expTimer;

	UFUNCTION(BlueprintCallable)
	void Explode();

	UFUNCTION(NetMulticast, Reliable)
	void PlayExplosionEffect();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
