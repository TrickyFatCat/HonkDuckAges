#include "HDAPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/HDAPlayerDamageManagerComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HonkDuckAges/Shared/Components/HDAArmorComponent.h"
#include "HonkDuckAges/Shared/Components/HDAHealthComponent.h"


AHDAPlayerCharacter::AHDAPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHDAHealthComponent>(TEXT("HealthComponent"));
	ArmorComponent = CreateDefaultSubobject<UHDAArmorComponent>(TEXT("ArmorComponent"));
	DamageManagerComponent = CreateDefaultSubobject<UHDAPlayerDamageManagerComponent>(TEXT("DamageManagerComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));

	bUseControllerRotationPitch = true;
}

void AHDAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(DefaultMappingContext != nullptr, TEXT("DefaultMappingContext wasn't set for %s"),
	           *GetActorNameOrLabel());

	ensureMsgf(MoveAction != nullptr, TEXT("MoveAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(AimAction != nullptr, TEXT("AimAction wasn't set for %s"), *GetActorNameOrLabel());
	
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
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AHDAPlayerCharacter::Aim);
	}
}

void AHDAPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementDirection = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementDirection.X);
	AddMovementInput(GetActorRightVector(), MovementDirection.Y);
}

void AHDAPlayerCharacter::Aim(const FInputActionValue& Value)
{
	const FVector2D AimDirection = Value.Get<FVector2D>();
	AddControllerYawInput(AimDirection.X);
	AddControllerPitchInput(AimDirection.Y);
}
