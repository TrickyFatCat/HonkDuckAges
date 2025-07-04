// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponData.h"
#include "Pickup/PickupAutomaticBase.h"
#include "HDAPickupAmmoBase.generated.h"

class UHDAPlayerWeaponManager;

UCLASS()
class HONKDUCKAGES_API AHDAPickupAmmoBase : public APickupAutomaticBase
{
	GENERATED_BODY()

public:
	AHDAPickupAmmoBase();

protected:
	virtual bool CanBeActivated_Implementation(AActor* Activator) override;

	virtual void HandleActivationSuccess_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup")
	EWeaponAmmoType AmmoType = EWeaponAmmoType::Gauge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup", meta=(ClampMin=1, UIMin=1))
	int32 AmmoToRestore = 1;

	UPROPERTY()
	TWeakObjectPtr<UHDAPlayerWeaponManager> WeaponManager = nullptr;
};
