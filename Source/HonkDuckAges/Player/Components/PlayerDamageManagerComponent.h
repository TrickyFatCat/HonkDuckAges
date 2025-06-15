// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Shared/Components/HDADamageManagerBase.h"
#include "PlayerDamageManagerComponent.generated.h"


class UHDAArmorComponent;

UCLASS(ClassGroup=(Player), meta=(BlueprintSpawnableComponent))
class HONKDUCKAGES_API UPlayerDamageManagerComponent : public UHDADamageManagerBase
{
	GENERATED_BODY()

public:
	UPlayerDamageManagerComponent();

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
