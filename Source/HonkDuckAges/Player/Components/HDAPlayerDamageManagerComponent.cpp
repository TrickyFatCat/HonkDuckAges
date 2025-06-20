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