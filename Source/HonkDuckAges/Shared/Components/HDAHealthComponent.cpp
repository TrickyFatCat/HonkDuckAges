// 


#include "HDAHealthComponent.h"


UHDAHealthComponent::UHDAHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHDAHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CurrentHealth.Value = DefaultHealth;
	CurrentHealth.MaxValue = DefaultHealth;
}

bool UHDAHealthComponent::IncreaseHealth(const int32 Amount)
{
	if (Amount <= 0 || CurrentHealth.ReachedMaxValue())
	{
		return false;
	}

	CurrentHealth.IncreaseValue(Amount);
	OnHealthIncreased.Broadcast(this, CurrentHealth, CurrentHealth.GetNormalizedValue());
	return true;
}

bool UHDAHealthComponent::DecreaseHealth(const int32 Amount)
{
	if (Amount <= 0 || CurrentHealth.ReachedMinValue())
	{
		return false;
	}

	CurrentHealth.DecreaseValue(Amount);
	CurrentHealth.ClampToMin();
	OnHealthDecreased.Broadcast(this, CurrentHealth, CurrentHealth.GetNormalizedValue());

	if (CurrentHealth.ReachedMinValue())
	{
		OnHealthReachedZero.Broadcast(this);
	}

	return true;
}

bool UHDAHealthComponent::IncreaseMaxHealth(const int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	CurrentHealth.IncreaseMaxValue(Amount);
	OnMaxHealthIncreased.Broadcast(this, CurrentHealth, CurrentHealth.GetNormalizedValue());

	const int32 DeltaHealth = CurrentHealth.MaxValue - CurrentHealth.Value;
	IncreaseHealth(DeltaHealth);
	return true;
}
