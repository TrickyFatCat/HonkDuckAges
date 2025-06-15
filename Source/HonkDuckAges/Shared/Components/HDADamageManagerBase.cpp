// 


#include "HDADamageManagerBase.h"

#include "HDAHealthComponent.h"

DEFINE_LOG_CATEGORY(LogDamageManagerComponent)

UHDADamageManagerBase::UHDADamageManagerBase()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	UActorComponent::SetAutoActivate(true);
}

void UHDADamageManagerBase::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();

	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	HealthComponent = GetOwner()->GetComponentByClass<UHDAHealthComponent>();

	ensureMsgf(IsValid(HealthComponent),
	           TEXT("Can't find health component in %s for DamageManagerComponent"),
	           *GetOwner()->GetActorNameOrLabel());

	GetOwner()->OnTakeAnyDamage.AddUniqueDynamic(this, &UHDADamageManagerBase::HandleDamageTaken);
}

void UHDADamageManagerBase::SetIsInvulnerable(const bool Value)
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

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void UHDADamageManagerBase::PrintLog(const FString& Message)
{
	UE_LOG(LogDamageManagerComponent, Log, TEXT("%s"), *Message)
}

void UHDADamageManagerBase::PrintWarning(const FString& Message)
{
	UE_LOG(LogDamageManagerComponent, Warning, TEXT("%s"), *Message)
}

void UHDADamageManagerBase::PrintError(const FString& Message)
{
	UE_LOG(LogDamageManagerComponent, Error, TEXT("%s"), *Message)
}
#endif

void UHDADamageManagerBase::HandleDamageTaken(AActor* DamagedActor,
                                              float Damage,
                                              const UDamageType* DamageType,
                                              AController* InstigatedBy,
                                              AActor* DamageCauser)
{
	if (!IsValid(HealthComponent) || HealthComponent->GetHealth().ReachedMinValue())
	{
		return;
	}

	HealthComponent->DecreaseHealth(Damage);
}
