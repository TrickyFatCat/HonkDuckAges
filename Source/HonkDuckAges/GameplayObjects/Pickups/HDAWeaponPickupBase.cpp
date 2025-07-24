// 


#include "HDAWeaponPickupBase.h"

#include "HonkDuckAges/Player/Components/HDAPlayerWeaponManager.h"


AHDAWeaponPickupBase::AHDAWeaponPickupBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	MeshComponent->SetupAttachment(GetRootComponent());
}

bool AHDAWeaponPickupBase::CanBeActivated_Implementation(AActor* Activator)
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

	return !WeaponManager->HasWeapon(WeaponToGive);
}

void AHDAWeaponPickupBase::HandleActivationSuccess_Implementation(AActor* Activator)
{
	WeaponManager->AddWeapon(WeaponToGive);
	WeaponManager->ChooseWeapon(WeaponToGive);
	WeaponManager->AddAmmo(WeaponManager->GetAmmoTypeForSlot(WeaponToGive), 9999);
}

