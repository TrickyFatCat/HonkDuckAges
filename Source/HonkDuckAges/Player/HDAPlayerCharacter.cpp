

#include "HDAPlayerCharacter.h"


AHDAPlayerCharacter::AHDAPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
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

