// 


#include "HDADamageManagerBase.h"

#include "HDAHealthComponent.h"


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

	if (!IsValid(HealthComponent))
	{
		
	}
	
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
	}
	else
	{
		OnInvulnerabilityDisabled.Broadcast(this);
	}
}

void UHDADamageManagerBase::HandleDamageTaken(AActor* DamagedActor,
                                              float Damage,
                                              const UDamageType* DamageType,
                                              AController* InstigatedBy,
                                              AActor* DamageCauser)
{
	if (!IsValid(HealthComponent))
	{
		return;
	}

	HealthComponent->DecreaseHealth(Damage);
}
