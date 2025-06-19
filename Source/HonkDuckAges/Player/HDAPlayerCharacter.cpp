#include "HDAPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/HDAPlayerDamageManagerComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/HDAPlayerMovementComponent.h"
#include "HonkDuckAges/Shared/Components/HDAArmorComponent.h"
#include "HonkDuckAges/Shared/Components/HDAHealthComponent.h"


AHDAPlayerCharacter::AHDAPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UHDAPlayerMovementComponent>(TEXT("CharMoveComp")))
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHDAHealthComponent>(TEXT("HealthComponent"));
	ArmorComponent = CreateDefaultSubobject<UHDAArmorComponent>(TEXT("ArmorComponent"));
	DamageManagerComponent = CreateDefaultSubobject<UHDAPlayerDamageManagerComponent>(TEXT("DamageManagerComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));
	CameraComponent->bUsePawnControlRotation = true;
}

void AHDAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(DefaultMappingContext != nullptr, TEXT("DefaultMappingContext wasn't set for %s"),
	           *GetActorNameOrLabel());

	ensureMsgf(MoveAction != nullptr, TEXT("MoveAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(AimAction != nullptr, TEXT("AimAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(JumpAction != nullptr, TEXT("JumpAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(DashAction != nullptr, TEXT("DashAction wasn't set for %s"), *GetActorNameOrLabel());

	const APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController))
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (EnhancedInputSubsystem)
		{
			EnhancedInputSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	PlayerMovementComponent = Cast<UHDAPlayerMovementComponent>(GetMovementComponent());

	ensureMsgf(PlayerMovementComponent != nullptr,
	           TEXT("%s movement component isn't changed to UHDAPlayerMovementComponent"),
	           *GetActorNameOrLabel());
}

void AHDAPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHDAPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHDAPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AHDAPlayerCharacter::StopMoving);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AHDAPlayerCharacter::Aim);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHDAPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this,
		                                   &AHDAPlayerCharacter::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AHDAPlayerCharacter::Dash);
	}
}

void AHDAPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D InputDirection = Value.Get<FVector2D>();
	MovementDirection = FVector(InputDirection.X, InputDirection.Y, 0.f);
	
	FVector ForwardDirection = GetActorForwardVector();
	ForwardDirection.Z = 0.f;
	ForwardDirection.Normalize();

	FVector RightDirection = GetActorRightVector();
	RightDirection.Z = 0.f;
	RightDirection.Normalize();
	
	AddMovementInput(ForwardDirection, MovementDirection.X);
	AddMovementInput(RightDirection, MovementDirection.Y);
}

void AHDAPlayerCharacter::StopMoving(const FInputActionValue& Value)
{
	MovementDirection = FVector::ZeroVector;
}

void AHDAPlayerCharacter::Aim(const FInputActionValue& Value)
{
	const FVector2D AimDirection = Value.Get<FVector2D>();
	AddControllerYawInput(AimDirection.X);
	AddControllerPitchInput(AimDirection.Y);
}

void AHDAPlayerCharacter::Dash()
{
	FVector ForwardDirection = GetActorForwardVector();
	ForwardDirection.Z = 0.f;
	ForwardDirection = ForwardDirection.GetSafeNormal();
	
	const FVector RightDirection = GetActorRightVector();
	FVector DashDirection = ForwardDirection;

	if (!MovementDirection.IsNearlyZero())
	{
		DashDirection = (ForwardDirection * MovementDirection.X + RightDirection * MovementDirection.Y).GetSafeNormal();
	}
	
	PlayerMovementComponent->StartDashing(DashDirection);
}
