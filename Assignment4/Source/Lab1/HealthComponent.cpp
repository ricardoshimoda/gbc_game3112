// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
AHealthComponent::AHealthComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;

	AActor* MyOwner = GetOwner();
	if(MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &AHealthComponent::HandleTakeAnyDamage );
	}
	
}

// Called every frame
void AHealthComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {

}

