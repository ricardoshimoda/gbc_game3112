// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthhComponent.h"

// Sets default values for this component's properties
UHealthhComponent::UHealthhComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthhComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthhComponent::HandleTakeAnyDamage);
	}

	// ...
	
}


// Called every frame
void UHealthhComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthhComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) {
	if (Damage <= 0.0f)
	{
		return;
	}
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed %s"), *FString::SanitizeFloat(Health));
}


