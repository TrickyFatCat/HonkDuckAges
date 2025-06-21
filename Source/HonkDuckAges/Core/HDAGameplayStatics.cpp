// 


#include "HDAGameplayStatics.h"

#include "HonkDuckAges/Shared/Components/HDALifeStateComponent.h"

bool UHDAGameplayStatics::ApplyHeal(AActor* Target, const int32 HealPower)
{
	if (!IsValid(Target) || HealPower <= 0)
	{
		return false;
	}

	UHDALifeStateComponent* LifeStateComponent = Target->FindComponentByClass<UHDALifeStateComponent>();

	if (!IsValid(LifeStateComponent))
	{
		return false;
	}

	return LifeStateComponent->IncreaseHealth(HealPower);
}

bool UHDAGameplayStatics::RestoreArmor(AActor* Target, const int32 Value)
{
	if (!IsValid(Target) || Value <= 0)
	{
		return false;
	}

	UHDALifeStateComponent* LifeStateComponent = Target->FindComponentByClass<UHDALifeStateComponent>();

	if (!IsValid(LifeStateComponent))
	{
		return false;
	}

	return LifeStateComponent->IncreaseArmor(Value);
}
