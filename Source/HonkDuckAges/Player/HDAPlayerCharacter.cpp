

#include "HDAPlayerCharacter.h"

#include "HonkDuckAges/Shared/Components/HDAArmorComponent.h"
#include "HonkDuckAges/Shared/Components/HDAHealthComponent.h"


AHDAPlayerCharacter::AHDAPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHDAHealthComponent>(TEXT("HealthComponent"));
	ArmorComponent = CreateDefaultSubobject<UHDAArmorComponent>(TEXT("ArmorComponent"));
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

