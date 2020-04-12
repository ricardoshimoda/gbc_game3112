// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSCharacter.h"
#include "TPSPlayer.generated.h"

UENUM()
enum class PlayerTeam : uint8
{
	None 		UMETA(DisplayName = "None"),
	TeamA 		UMETA(DisplayName = "TeamA"),
	TeamB 		UMETA(DisplayName = "TeamB")
};

class UCameraComponent;
class USpringArmComponent;
class UUserWidget;
UCLASS()
class THIRDPERSONSHOOTER_API ATPSPlayer : public ATPSCharacter
{
	GENERATED_BODY()
public:
	ATPSPlayer();
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "PlayerProperties")
	TEnumAsByte<PlayerTeam> currentPlayerTeam = PlayerTeam::None;
protected:

	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerProperties", meta = (ClampMin = 30, ClampMax = 120))
	float defaultFOV;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerProperties", meta = (ClampMin = 30, ClampMax = 120))
	float zoomedFOV;
	UPROPERTY(replicated)
	float targetFOV;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerProperties")
	float zoomSpeed = 20;

	virtual void StartZoom_Implementation() override;
	virtual void EndZoom_Implementation() override;
	virtual void StartFire_Implementation() override;
	virtual void EndFire_Implementation() override;


	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
public:
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void RandomFunctionOver();
	void RandomFunctionOver_Implementation();
	bool RandomFunctionOver_Validate();

};
