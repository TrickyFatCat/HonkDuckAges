// 


#include "HDAPlayerWeaponBase.h"

#include "Components/ArrowComponent.h"


AHDAPlayerWeaponBase::AHDAPlayerWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

#if WITH_EDITOR
	ForwardVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardVector"));
	ForwardVector->SetupAttachment(GetRootComponent());
#endif
}

void AHDAPlayerWeaponBase::StartShooting()
{
}

void AHDAPlayerWeaponBase::StopShooting()
{
}

void AHDAPlayerWeaponBase::MakeShot()
{
}
