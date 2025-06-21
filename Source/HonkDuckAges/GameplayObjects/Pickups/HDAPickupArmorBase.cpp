// 


#include "HDAPickupArmorBase.h"

#include "HonkDuckAges/Shared/Components/HDALifeStateComponent.h"


AHDAPickupArmorBase::AHDAPickupArmorBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AHDAPickupArmorBase::CanBeActivated_Implementation(AActor* Activator)
{
	if (!IsValid(Activator))
	{
		return false;
	}

	LifeStateComponent = Activator->GetComponentByClass<UHDALifeStateComponent>();

	if (!LifeStateComponent.IsValid())
	{
		return false;
	}

	return !LifeStateComponent->GetArmor().ReachedMaxValue();
}

void AHDAPickupArmorBase::HandleActivationSuccess_Implementation(AActor* Activator)
{
	LifeStateComponent->IncreaseArmor(ArmorToRestore);
}
