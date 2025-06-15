

#include "HDAPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/HDAPlayerDamageManagerComponent.h"
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
}

void AHDAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDAPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHDAPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

