// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup=(TPS), meta=(BlueprintSpawnableComponent))
class LAB1_API AHealthComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly, Category = "HealthComponent")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float MaxHealth;
	void HandleTakeAnyDamage (AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
