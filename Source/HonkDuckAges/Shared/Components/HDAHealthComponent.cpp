// 


#include "HDAHealthComponent.h"

DEFINE_LOG_CATEGORY(LogHealthComponent)

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

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Health of %s increased by %d. NewHealth: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Amount,
	                         CurrentHealth.Value,
	                         CurrentHealth.MaxValue,
	                         CurrentHealth.GetNormalizedValue() * 100.0f));
#endif
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

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Health of %s decreased by %d. NewHealth: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Amount,
	                         CurrentHealth.Value,
	                         CurrentHealth.MaxValue,
	                         CurrentHealth.GetNormalizedValue() * 100.0f));
#endif

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

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("MaxHealth of %s increased by %d and equal to %d"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Amount,
	                         CurrentHealth.MaxValue));
#endif

	const int32 DeltaHealth = CurrentHealth.MaxValue - CurrentHealth.Value;
	IncreaseHealth(DeltaHealth);
	return true;
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void UHDAHealthComponent::PrintLog(const FString& Message)
{
	UE_LOG(LogHealthComponent, Log, TEXT("%s"), *Message);
}

void UHDAHealthComponent::PrintWarning(const FString& Message)
{
	UE_LOG(LogHealthComponent, Warning, TEXT("%s"), *Message);
}

void UHDAHealthComponent::PrintError(const FString& Message)
{
	UE_LOG(LogHealthComponent, Error, TEXT("%s"), *Message);
}
#endif
