// 


#include "HDAGameplayStatics.h"

#include "HonkDuckAges/Shared/Components/HDALifeStateComponent.h"
#include "LockKey/KeyringComponent.h"
#include "LockKey/LockKeyType.h"

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

bool UHDAGameplayStatics::CanGiveKey(AActor* Target, const TSubclassOf<ULockKeyType> KeyClass)
{
	if (!IsValid(Target) || !IsValid(KeyClass))
	{
		return false;
	}

	UKeyringComponent* KeyringComponent = Target->FindComponentByClass<UKeyringComponent>();

	if (!IsValid(KeyringComponent))
	{
		return false;
	}

	return IKeyringInterface::Execute_HasLockKey(KeyringComponent, KeyClass);
}

void UHDAGameplayStatics::GiveKey(AActor* Target, const TSubclassOf<ULockKeyType> KeyClass)
{
	if (!IsValid(Target) || !IsValid(KeyClass))
	{
		return;
	}

	UKeyringComponent* KeyringComponent = Target->FindComponentByClass<UKeyringComponent>();

	if (!IsValid(KeyringComponent))
	{
		return;
	}

	IKeyringInterface::Execute_AddLockKey(KeyringComponent, KeyClass);
}

