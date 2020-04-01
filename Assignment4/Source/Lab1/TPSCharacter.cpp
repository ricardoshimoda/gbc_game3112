// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSCharacter.h"
//Additions
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ADDITION: "Unreal Way" of creating objects (templated factory methods)
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComp->SetupAttachment(SpringArmComp);

	FinalWeaponFireMode = false;
}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	EndZoom();
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ATPSWeapon>(StarterWeaponClass, spawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(Cast<USceneComponent>(GetMesh()), 
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			WeaponSocketName);
	}
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	dt = DeltaTime;
	if (bInCover) 
	{
		if (bIsAiming) {
			GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		}
		else 
		{
			if (overlappingCoverVolume) {
				GetMesh()->SetWorldRotation(overlappingCoverVolume->GetComponentRotation().Add(0, 40, 0));
			}
		}
	}
	else 
	{
		GetMesh()->SetRelativeRotation(FRotator(0.0f,-90.0f,0.0f));
	}


}

//MoveForward Definition:
void ATPSCharacter::MoveForward(float Val) {

	if (!bInCover) {
		GetCharacterMovement()->AddInputVector(GetActorForwardVector()*Val);
	}
}

void ATPSCharacter::MoveRight(float Val) {

	if (bInCover)
	{
		FVector targetPosition = GetActorLocation() + overlappingCoverVolume->GetRightVector() * Val * dt;
		FVector offsetFromCoverCenter = targetPosition - overlappingCoverVolume->GetComponentLocation();
		float dotProduct = FVector::DotProduct(offsetFromCoverCenter, overlappingCoverVolume->GetRightVector());
		bool isTargetInRange = abs(dotProduct) < overlappingCoverVolume->GetScaledBoxExtent().Y - 50;
		bool isMovingTowardsCenter = dotProduct * Val < 0;
		if (isTargetInRange || isMovingTowardsCenter) {
			GetCharacterMovement()->AddInputVector(overlappingCoverVolume->GetRightVector()*Val);
		}
	}
	else {
		GetCharacterMovement()->AddInputVector(GetActorRightVector()*Val);
	}
}

void ATPSCharacter::BeginCrouch() {
	Crouch();
}

void ATPSCharacter::EndCrouch() {
	UnCrouch();
}

void ATPSCharacter::Jump() {
	ACharacter::Jump();
}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//function binding
	//CAREFUL: SOME FUNCTIONS ARE PARENT-CLASS-BASED
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);
	//pitch: up-down, Yaw: Left-right, Roll: rotation along forward axis
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookSideWays", this, &ACharacter::AddControllerYawInput);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("UnCrouch", IE_Released, this, &ATPSCharacter::EndCrouch);
	
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSCharacter::StartZoom);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPSCharacter::EndZoom);

	PlayerInputComponent->BindAction("ChangeWeaponMode", IE_Pressed, this, &ATPSCharacter::ChangeWeaponMode);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ATPSCharacter::FireWeapon);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ATPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ATPSCharacter::StopFire);

	PlayerInputComponent->BindAction("TakeCover", IE_Pressed, this, &ATPSCharacter::TakeCover);

}
void ATPSCharacter::ChangeWeaponMode() 
{
	UE_LOG(LogTemp, Warning, TEXT("Changing Fire Weapon Mode"));
	if (CurrentWeapon)
	{
		FinalWeaponFireMode = !FinalWeaponFireMode;
		CurrentWeapon->ChangeWeaponFireMode();
	}
}

FVector ATPSCharacter::GetPawnViewLocation() const {

	if (CameraComp) return CameraComp->GetComponentLocation();

	return Super::GetPawnViewLocation();
}

void ATPSCharacter::StartZoom() {
	bIsAiming = true;
	CameraComp->SetFieldOfView(zoomedFoV);
}

void ATPSCharacter::EndZoom() {
	bIsAiming = false;
	CameraComp->SetFieldOfView(defaultFoV);
}

void ATPSCharacter::FireWeapon() {
	if (CurrentWeapon && !FinalWeaponFireMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling Fire Weapon"));
		CurrentWeapon->Fire();
	}
}
void ATPSCharacter::StartFire() {
	if (CurrentWeapon && FinalWeaponFireMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling Start Fire "));
		CurrentWeapon->StartFire();
	}
}
void ATPSCharacter::StopFire() {
	if (CurrentWeapon && FinalWeaponFireMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Calling Stop Fire "));
		CurrentWeapon->StopFire();
	}
}

void ATPSCharacter::TakeCover()
{
	if (bInCover)
	{
		bInCover = false;
		return;
	}
	if (overlappingCoverVolume) 
	{
		FVector overlappingForward = overlappingCoverVolume->GetForwardVector();
		FVector lineTraceStart = GetActorLocation();
		FVector lineTraceEnd = GetActorLocation() + overlappingForward * 10000;
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, lineTraceStart, lineTraceEnd, ECC_Visibility)) {
			FVector hitLocation = Hit.Location;
			FVector finalActorLocation = hitLocation - GetCapsuleComponent()->GetScaledCapsuleRadius() * overlappingForward;
			SetActorLocation(finalActorLocation);
			bInCover = true;
		}

	}
}