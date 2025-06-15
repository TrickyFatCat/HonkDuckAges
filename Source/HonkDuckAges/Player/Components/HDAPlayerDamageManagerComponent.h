// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Shared/Components/HDADamageManagerBase.h"
#include "HDAPlayerDamageManagerComponent.generated.h"


class UHDAArmorComponent;

UCLASS(ClassGroup=(Player))
class HONKDUCKAGES_API UHDAPlayerDamageManagerComponent : public UHDADamageManagerBase
{
	GENERATED_BODY()

public:
	UHDAPlayerDamageManagerComponent();

protected:
	virtual void InitializeComponent() override;

	UPROPERTY()
	TObjectPtr<UHDAArmorComponent> ArmorComponent = nullptr;

private:
	virtual void HandleDamageTaken(AActor* DamagedActor,
	                               float Damage,
	                               const UDamageType* DamageType,
	                               AController* InstigatedBy,
	                               AActor* DamageCauser) override;
};
