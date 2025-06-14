

#include "HDEPlayerCharacter.h"


AHDEPlayerCharacter::AHDEPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHDEPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDEPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHDEPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

