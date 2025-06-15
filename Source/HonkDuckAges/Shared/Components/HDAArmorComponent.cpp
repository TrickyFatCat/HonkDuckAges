//l 


#include "HDAArmorComponent.h"

DEFINE_LOG_CATEGORY(LogArmorComponent)

UHDAArmorComponent::UHDAArmorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	UActorComponent::SetAutoActivate(true);
}

void UHDAArmorComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

bool UHDAArmorComponent::IncreaseArmor(const int32 Amount)
{
	if (Amount <= 0 || CurrentArmor.ReachedMaxValue())
	{
		return false;
	}

	CurrentArmor.IncreaseValue(Amount);
	OnArmorIncreased.Broadcast(this, CurrentArmor, CurrentArmor.GetNormalizedValue());

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Armor of %s increased by %d. NewArmor: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Amount,
	                         CurrentArmor.Value,
	                         CurrentArmor.MaxValue,
	                         CurrentArmor.GetNormalizedValue() * 100.0f));
#endif
	return true;
}

bool UHDAArmorComponent::DecreaseArmor(const int32 Amount)
{
	if (Amount <= 0 || CurrentArmor.ReachedMinValue())
	{
		return false;
	}

	CurrentArmor.DecreaseValue(Amount);
	CurrentArmor.ClampToMin();
	OnArmorDecreased.Broadcast(this, CurrentArmor, CurrentArmor.GetNormalizedValue());

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Armor of %s decreased by %d. NewArmor: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Amount,
	                         CurrentArmor.Value,
	                         CurrentArmor.MaxValue,
	                         CurrentArmor.GetNormalizedValue() * 100.0f));
#endif

	if (CurrentArmor.ReachedMinValue())
	{
		OnArmorReachedZero.Broadcast(this);
	}

	return true;
}

bool UHDAArmorComponent::IncreaseMaxArmor(const int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	CurrentArmor.IncreaseMaxValue(Amount);
	OnMaxArmorIncreased.Broadcast(this, CurrentArmor, CurrentArmor.GetNormalizedValue());

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("MaxArmor of %s increased by %d and equal to %d"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Amount,
	                         CurrentArmor.MaxValue));
#endif

	const int32 DeltaArmor = CurrentArmor.MaxValue - CurrentArmor.Value;
	IncreaseArmor(DeltaArmor);
	return true;
}

#if WITH_EDITOR && !UE_BUILD_SHIPPING
void UHDAArmorComponent::PrintLog(const FString& Message)
{
	UE_LOG(LogArmorComponent, Log, TEXT("%s"), *Message);
}

void UHDAArmorComponent::PrintWarning(const FString& Message)
{
	UE_LOG(LogArmorComponent, Warning, TEXT("%s"), *Message);
}

void UHDAArmorComponent::PrintError(const FString& Message)
{
	UE_LOG(LogArmorComponent, Error, TEXT("%s"), *Message);
}
#endif
