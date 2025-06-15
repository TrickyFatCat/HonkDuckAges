// 


#include "PlayerDamageManagerComponent.h"

#include "HonkDuckAges/Shared/Components/HDAArmorComponent.h"
#include "HonkDuckAges/Shared/Components/HDAHealthComponent.h"


UPlayerDamageManagerComponent::UPlayerDamageManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerDamageManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();

	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	ArmorComponent = GetOwner()->GetComponentByClass<UHDAArmorComponent>();

	if (!IsValid(ArmorComponent))
	{
	}
}

void UPlayerDamageManagerComponent::HandleDamageTaken(AActor* DamagedActor,
                                                      float Damage,
                                                      const UDamageType* DamageType,
                                                      AController* InstigatedBy,
                                                      AActor* DamageCauser)
{
	if (!IsValid(ArmorComponent) || !IsValid(HealthComponent))
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
