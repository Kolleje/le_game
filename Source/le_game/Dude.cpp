// Copyright Epic Games, Inc. All Rights Reserved.

#include "Dude.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
//#include "PaperSpriteComponent.h"
#include "PaperCharacter.h"
#include "Components/ArrowComponent.h"
#include "DirectionHelper.h"
#include "Constants.h"
//#include "GameFramework/Character.h"

//////////////////////////////////////////////////////////////////////////
// ADude

ADude::ADude()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->SetRelativeRotation(FRotator(-60.f, -90.f, 0.f));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
	MovementDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MovementDirectionArrow"));
	MovementDirectionArrow->AttachToComponent(RootComponent, TransformRules);
	//MovementDirectionArrow->AttachTo(RootComponent);
	
	FacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingArrow"));
	FacingArrow->AttachToComponent(RootComponent, TransformRules);
	//FacingArrow->AttachTo(RootComponent);

	//GetCharacterMovement()->

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	GetCharacterMovement()->MaxWalkSpeed = 200;
	base_speed = GetCharacterMovement()->MaxWalkSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADude::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveX", this, &ADude::MoveX);
	PlayerInputComponent->BindAxis("MoveY", this, &ADude::MoveY);

	PlayerInputComponent->BindAxis("FaceX", this, &ADude::FaceX);
	PlayerInputComponent->BindAxis("FaceY", this, &ADude::FaceY);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ADude::OnSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ADude::OnSprintEnd);
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADude::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADude::LookUpAtRate);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ADude::OnAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ADude::OnAimEnd);
}


void ADude::LookAtDirection(FVector2D Vector)
{
	FString xy = TEXT("aiming: ");
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, xy);
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADude::TurnAtRate(float Rate)
{
	right_axis = Rate;
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADude::LookUpAtRate(float Rate)
{
	top_axis = -1 * Rate;
	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADude::MoveX(float Value)
{
	if (Controller != nullptr)
	{
		movement_dir.SetComponentForAxis(EAxis::X, Value);
	}
}

void ADude::MoveY(float Value)
{
	if (Controller != nullptr)
	{
		movement_dir.SetComponentForAxis(EAxis::Y, -Value);

	}
}

void ADude::FaceX(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FVector Dir = FVector(1.f, 0.f, 0.f);
		look_dir.SetComponentForAxis(EAxis::X, Value);
		_aiming = true;
	}
}

void ADude::FaceY(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FVector Dir = FVector(0.f, 1.f, 0.f);
		look_dir.SetComponentForAxis(EAxis::Y, Value);
		_aiming = true;
	}
}

void ADude::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (_sprinting) _aiming = false;

	if (_aiming) {

	}
	else {
		if (!movement_dir.IsNearlyZero()) look_dir.Set(movement_dir.GetComponentForAxis(EAxis::X), movement_dir.GetComponentForAxis(EAxis::Y), movement_dir.GetComponentForAxis(EAxis::Z));
	}

	_aiming = false;

	if (!movement_dir.IsNearlyZero()) MovementDirectionArrow->SetWorldRotation(movement_dir.Rotation());
	else MovementDirectionArrow->SetWorldRotation(look_dir.Rotation());
	//DirectionArrow->AddWorldRotation(FRotator(0.f, 0.f, 25.f));
	FacingArrow->SetWorldRotation(look_dir.Rotation());

	direction_offset = DirectionHelper::GetMovementFacingOffset(movement_dir, look_dir);

	float speed = movement_dir.Size();

	/*FString speedText = TEXT("speed: ") + FString::FromInt(direction_offset);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, speedText);*/


	if (direction_offset == LG_SIDE) speed = speed * sideways_factor;
	if (direction_offset == LG_BACK) speed = speed * backwards_factor;

	movement_state = DirectionHelper::GetMovementState(speed, base_speed, direction_offset);

	if (!movement_dir.IsNearlyZero()) AddMovementInput(movement_dir, speed);

	tick_count++;
	time_passed += DeltaTime;
	if (GEngine && tick_count >= 200) {
		float fps = tick_count / time_passed;
		FString xy = TEXT("fps: ") + FString::SanitizeFloat(fps);
		//FString xy = TEXT("right: ") + FString::SanitizeFloat(right_axis) + TEXT("top: ") + FString::SanitizeFloat(top_axis);
		tick_count = 0;
		time_passed = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, xy);
	}
}

void ADude::OnAim() {
	_aiming = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	//GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ADude::OnAimEnd() {
	_aiming = false;
	//GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ADude::OnSprint() {
	GetCharacterMovement()->MaxWalkSpeed = 1.5 * base_speed;
	_sprinting = true;
}

void ADude::OnSprintEnd() {
	GetCharacterMovement()->MaxWalkSpeed = base_speed;
	_sprinting = false;
}

int ADude::GetMovementDirectionKey() {
	return DirectionHelper::DirectionKeyFromVector(movement_dir);
}

int ADude::GetFacingDirectionKey() {
	return DirectionHelper::DirectionKeyFromVector(look_dir);
}