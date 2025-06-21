// 


#include "HDAPickupHealthBase.h"

#include "HonkDuckAges/Shared/Components/HDALifeStateComponent.h"


AHDAPickupHealthBase::AHDAPickupHealthBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AHDAPickupHealthBase::CanBeActivated_Implementation(AActor* Activator)
{
	LifeStateComponent = Activator->GetComponentByClass<UHDALifeStateComponent>();

	if (!LifeStateComponent.IsValid())
	{
		return false;
	}

	return !LifeStateComponent->GetHealth().ReachedMaxValue();
}

void AHDAPickupHealthBase::HandleActivationSuccess_Implementation(AActor* Activator)
{
	LifeStateComponent->IncreaseHealth(HealPower);
}
