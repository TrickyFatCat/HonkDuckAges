// 


#include "HDAPickupAmmoBase.h"

#include "HonkDuckAges/Player/Components/HDAPlayerWeaponManager.h"


AHDAPickupAmmoBase::AHDAPickupAmmoBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->SetupAttachment(GetRootComponent());
}

bool AHDAPickupAmmoBase::CanBeActivated_Implementation(AActor* Activator)
{
	if (!IsValid(Activator))
	{
		return false;
	}

	WeaponManager = Activator->GetComponentByClass<UHDAPlayerWeaponManager>();

	if (!WeaponManager.IsValid())
	{
		return false;
	}

	FTrickyPropertyInt Ammo;
	WeaponManager->GetAmmo(AmmoType, Ammo);
	
	return !Ammo.ReachedMaxValue();
}

void AHDAPickupAmmoBase::HandleActivationSuccess_Implementation(AActor* Activator)
{
	WeaponManager->AddAmmo(AmmoType, AmmoToRestore);
}
