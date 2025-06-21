// 

#pragma once

#include "CoreMinimal.h"
#include "Pickup/PickupAutomaticBase.h"
#include "HDAPickupHealthBase.generated.h"

class UHDALifeStateComponent;

UCLASS(Abstract, HideDropdown)
class HONKDUCKAGES_API AHDAPickupHealthBase : public APickupAutomaticBase
{
	GENERATED_BODY()

public:
	AHDAPickupHealthBase();

protected:
	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Pickup",
		meta=(ClampMin=1, UIMin=1))
	int32 HealPower = 25;

	UPROPERTY()
	TWeakObjectPtr<UHDALifeStateComponent> LifeStateComponent = nullptr;

	virtual bool CanBeActivated_Implementation(AActor* Activator) override;

	virtual void HandleActivationSuccess_Implementation(AActor* Activator) override;
};
