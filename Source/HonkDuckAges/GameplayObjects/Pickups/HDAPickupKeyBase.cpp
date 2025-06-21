// 


#include "HDAPickupKeyBase.h"

#include "LockKey/KeyringComponent.h"


AHDAPickupKeyBase::AHDAPickupKeyBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AHDAPickupKeyBase::CanBeActivated_Implementation(AActor* Activator)
{
	if (!IsValid(Activator))
	{
		return false;
	}

	KeyringComponent = Activator->GetComponentByClass<UKeyringComponent>();

	if (!KeyringComponent.IsValid())
	{
		return false;
	}

	return !IKeyringInterface::Execute_HasLockKey(KeyringComponent.Get(), KeyClass);
}

void AHDAPickupKeyBase::HandleActivationSuccess_Implementation(AActor* Activator)
{
	if (!KeyringComponent.IsValid())
	{
		return;
	}

	IKeyringInterface::Execute_AddLockKey(KeyringComponent.Get(), KeyClass);
}
