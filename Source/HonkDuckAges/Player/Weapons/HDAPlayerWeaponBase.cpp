// 


#include "HDAPlayerWeaponBase.h"

#include "HDAPlayerProjectileBase.h"
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

void AHDAPlayerWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		ensureMsgf(IsValid(ProjectileClass) && BulletType == EWeaponBulletType::Projectile,
		           TEXT("Invalid projectile class in %s"), *GetActorNameOrLabel());
	}
}

void AHDAPlayerWeaponBase::StartShooting(const FVector& TargetPoint)
{
	MakeShot(TargetPoint);
}

void AHDAPlayerWeaponBase::StopShooting()
{
}

void AHDAPlayerWeaponBase::MakeShot(const FVector& TargetPoint)
{
	switch (BulletType)
	{
	case EWeaponBulletType::Projectile:
		if (!IsValid(ProjectileClass))
		{
			return;
		}

		for (int32 i = 0; i <= BulletsPerShot; ++i)
		{
			SpawnProjectile(TargetPoint);
		}
		break;

	case EWeaponBulletType::Trace:
		break;
	}

	OnPlayerWeaponShot.Broadcast(this);
}

void AHDAPlayerWeaponBase::SpawnProjectile(const FVector& TargetPoint)
{
}
