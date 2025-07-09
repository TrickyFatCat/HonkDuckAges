// 


#include "HDAPlayerWeaponBase.h"

#include "HDAPlayerProjectileBase.h"
#include "HDAWeaponStateController.h"
#include "Components/ArrowComponent.h"
#include "HonkDuckAges/Player/Components/HDAPlayerWeaponManager.h"


AHDAPlayerWeaponBase::AHDAPlayerWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponStateController = CreateDefaultSubobject<UHDAWeaponStateController>(TEXT("WeaponStateController"));

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
		if (BulletType == EWeaponBulletType::Projectile)
		{
			ensureMsgf(IsValid(ProjectileClass),
			           TEXT("Invalid projectile class in %s"), *GetActorNameOrLabel());
		}

		ensureMsgf(OwningWeaponManager.IsValid(),
		           TEXT("Invalid owning weapon manager for %s"), *GetActorNameOrLabel());
	}
}

void AHDAPlayerWeaponBase::StartShooting(const FVector& TargetPoint)
{
	if (GetCurrentState() == EWeaponState::OutOfAmmo)
	{
		return;
	}

	WeaponStateController->TransitToShooting(true);
	MakeShot(TargetPoint);

	if (WeaponMode == EWeaponMode::SemiAuto)
	{
		StopShooting();
	}
}

void AHDAPlayerWeaponBase::StopShooting()
{
	if (GetCurrentState() != EWeaponState::Shooting)
	{
		return;
	}

	const bool bIsOutOfAmmo = OwningWeaponManager->GetCurrentAmmo().ReachedMinValue();
	bIsOutOfAmmo ? WeaponStateController->TransitToOutOfAmmo(true) : WeaponStateController->TransitToIdle(true);
}

void AHDAPlayerWeaponBase::TransitToIdle() const
{
	WeaponStateController->TransitToIdle(true);
}

void AHDAPlayerWeaponBase::SetOwningWeaponManager(UHDAPlayerWeaponManager* NewManager)
{
	if (!IsValid(NewManager) || OwningWeaponManager.IsValid())
	{
		return;
	}

	OwningWeaponManager = NewManager;
	OwningWeaponManager->OnAmmoIncreased.AddUniqueDynamic(this, &AHDAPlayerWeaponBase::HandleAmmoIncreased);
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

void AHDAPlayerWeaponBase::HandleAmmoIncreased(UHDAPlayerWeaponManager* Component,
                                               EWeaponAmmoType AmmoType,
                                               const FTrickyPropertyInt& Ammo,
                                               int32 DeltaValue)
{
	if (!Ammo.ReachedMinValue() && GetCurrentState() == EWeaponState::OutOfAmmo)
	{
		WeaponStateController->TransitToIdle(true);
	}
}
