// 


#include "HDAPlayerDamageManagerComponent.h"

#include "HonkDuckAges/Player/HDAPlayerCharacter.h"
#include "HonkDuckAges/Shared/Components/HDAArmorComponent.h"
#include "HonkDuckAges/Shared/Components/HDAHealthComponent.h"


UHDAPlayerDamageManagerComponent::UHDAPlayerDamageManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHDAPlayerDamageManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();

	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	ArmorComponent = GetOwner()->GetComponentByClass<UHDAArmorComponent>();

	ensureMsgf(IsValid(ArmorComponent),
	           TEXT("Can't find ArmorComponent in %s for DamageManagerComponent"),
	           *GetOwner()->GetActorNameOrLabel());

	ensureMsgf(GetOwner()->IsA(AHDAPlayerCharacter::StaticClass()),
	           TEXT("Please don't use PlayerDamageManagerComponent in non player classes like %s"),
	           *GetOwner()->GetActorNameOrLabel());
}

void UHDAPlayerDamageManagerComponent::HandleDamageTaken(AActor* DamagedActor,
                                                         float Damage,
                                                         const UDamageType* DamageType,
                                                         AController* InstigatedBy,
                                                         AActor* DamageCauser)
{
	if (!IsValid(ArmorComponent) || !IsValid(HealthComponent) || GetIsInvulnerable())
	{
		return;
	}

	int32 RemainingDamage = Damage;
	const FTrickyPropertyInt Armor = ArmorComponent->GetArmor();

	if (!Armor.ReachedMinValue())
	{
		if (RemainingDamage > Armor.Value)
		{
			RemainingDamage -= Armor.Value;
			ArmorComponent->DecreaseArmor(Armor.Value);
		}
		else
		{
			ArmorComponent->DecreaseArmor(RemainingDamage);
			return;
		}
	}

	HealthComponent->DecreaseHealth(RemainingDamage);
}
