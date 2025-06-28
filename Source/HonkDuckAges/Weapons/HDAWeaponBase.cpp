// 


#include "HDAWeaponBase.h"

#include "Components/ArrowComponent.h"


AHDAWeaponBase::AHDAWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

#if WITH_EDITOR
	ForwardVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardVector"));
	ForwardVector->SetupAttachment(GetRootComponent());
#endif
}

void AHDAWeaponBase::StartShooting()
{
}

void AHDAWeaponBase::StopShooting()
{
}

void AHDAWeaponBase::MakeShot()
{
}
