// 


#include "HDALifeStateComponent.h"

DEFINE_LOG_CATEGORY(LogLifeStateComponent)

UHDALifeStateComponent::UHDALifeStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	UActorComponent::SetAutoActivate(true);
}

void UHDALifeStateComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Health.Value = DefaultHealth;
	Health.MaxValue = DefaultHealth;

	Armor.Value = DefaultArmor;
	Armor.MaxValue = DefaultArmor;

	GetOwner()->OnTakeAnyDamage.AddUniqueDynamic(this, &UHDALifeStateComponent::HandleDamageTaken);
}

bool UHDALifeStateComponent::IncreaseHealth(const int32 Value)
{
	if (Value <= 0 || Health.ReachedMaxValue())
	{
		return false;
	}

	Health.IncreaseValue(Value);
	Health.ClampToMax();

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Health of %s increased by %d. New Health: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Value,
	                         Health.Value,
	                         Health.MaxValue,
	                         Health.GetNormalizedValue() * 100.0f));
#endif

	OnHealthIncreased.Broadcast(this, Health, Health.GetNormalizedValue());
	return true;
}

bool UHDALifeStateComponent::IncreaseMaxHealth(const int32 Value)
{
	if (Value <= 0)
	{
		return false;
	}

	Health.IncreaseMaxValue(Value);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("MaxHealth of %s increased by %d. New Max Health: %d"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Value,
	                         Health.MaxValue));
#endif

	OnMaxHealthIncreased.Broadcast(this, Health, Health.GetNormalizedValue());
	IncreaseHealth(Health.MaxValue - Health.Value);
	return true;
}

bool UHDALifeStateComponent::IncreaseArmor(const int32 Value)
{
	if (Value <= 0 || Armor.ReachedMaxValue())
	{
		return false;
	}

	Armor.IncreaseValue(Value);
	Armor.ClampToMax();

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Armor of %s increased by %d. New Armor: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Value,
	                         Armor.Value,
	                         Armor.MaxValue,
	                         Armor.GetNormalizedValue() * 100.0f));
#endif

	OnArmorIncreased.Broadcast(this, Armor, Armor.GetNormalizedValue());
	return true;
}

bool UHDALifeStateComponent::IncreaseMaxArmor(const int32 Value)
{
	if (Value <= 0)
	{
		return false;
	}

	Armor.IncreaseMaxValue(Value);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("MaxArmor of %s increased by %d. New Max Armor: %d"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Value,
	                         Armor.MaxValue));
#endif

	OnMaxArmorIncreased.Broadcast(this, Armor, Armor.GetNormalizedValue());
	IncreaseArmor(Armor.MaxValue - Armor.Value);
	return true;
}

void UHDALifeStateComponent::SetIsInvulnerable(const bool Value)
{
	if (bIsInvulnerable == Value)
	{
		return;
	}

	bIsInvulnerable = Value;

	if (bIsInvulnerable)
	{
		OnInvulnerabilityEnabled.Broadcast(this);
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		PrintLog(FString::Printf(TEXT("Invulnerability enabled for %s"), *GetOwner()->GetActorNameOrLabel()));
#endif
	}
	else
	{
		OnInvulnerabilityDisabled.Broadcast(this);
#if WITH_EDITOR && !UE_BUILD_SHIPPING
		PrintLog(FString::Printf(TEXT("Invulnerability disabled for %s"), *GetOwner()->GetActorNameOrLabel()));
#endif
	}
}

void UHDALifeStateComponent::HandleDamageTaken(AActor* DamagedActor,
                                               float Damage,
                                               const UDamageType* DamageType,
                                               AController* InstigatedBy,
                                               AActor* DamageCauser)
{
	if (bIsInvulnerable)
	{
		return;
	}

	const int32 ArmorDamage = Damage > Armor.Value ? Armor.Value : static_cast<int32>(Damage);
	const int32 HealthDamage = FMath::Max(0, static_cast<int32>(Damage) - ArmorDamage);
	DecreaseArmor(ArmorDamage);
	DecreaseHealth(HealthDamage);
}

void UHDALifeStateComponent::DecreaseArmor(const int32 Value)
{
	if (Value <= 0 || Armor.ReachedMinValue())
	{
		return;
	}

	Armor.DecreaseValue(Value);
	Armor.ClampToMin();

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Armor of %s decreased by %d. New Armor: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Value,
	                         Armor.Value,
	                         Armor.MaxValue,
	                         Armor.GetNormalizedValue() * 100.0f));
#endif

	OnArmorDecreased.Broadcast(this, Armor, Armor.GetNormalizedValue());

	if (Armor.ReachedMinValue())
	{
		OnArmorReachedZero.Broadcast(this);
	}
}

void UHDALifeStateComponent::DecreaseHealth(const int32 Value)
{
	if (Value <= 0 || Health.ReachedMinValue())
	{
		return;
	}

	Health.DecreaseValue(Value);
	Health.ClampToMin();

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintLog(FString::Printf(TEXT("Health of %s decreased by %d. New Health: %d/%d | %.2f %%"),
	                         *GetOwner()->GetActorNameOrLabel(),
	                         Value,
	                         Health.Value,
	                         Health.MaxValue,
	                         Health.GetNormalizedValue() * 100.0f));
#endif

	OnHealthDecreased.Broadcast(this, Health, Health.GetNormalizedValue());

	if (Health.ReachedMinValue())
	{
		OnHealthReachedZero.Broadcast(this);
	}
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void UHDALifeStateComponent::PrintLog(const FString& Message)
{
	UE_LOG(LogLifeStateComponent, Log, TEXT("%s"), *Message)
}

void UHDALifeStateComponent::PrintWarning(const FString& Message)
{
	UE_LOG(LogLifeStateComponent, Warning, TEXT("%s"), *Message)
}

void UHDALifeStateComponent::PrintError(const FString& Message)
{
	UE_LOG(LogLifeStateComponent, Error, TEXT("%s"), *Message)
}
#endif
