// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponData.h"
#include "Pickup/PickupAutomaticBase.h"
#include "HDAWeaponPickupBase.generated.h"

class UHDAPlayerWeaponManager;

UCLASS()
class HONKDUCKAGES_API AHDAWeaponPickupBase : public APickupAutomaticBase
{
	GENERATED_BODY()

public:
	AHDAWeaponPickupBase();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;
	
	virtual bool CanBeActivated_Implementation(AActor* Activator) override;

	virtual void HandleActivationSuccess_Implementation(AActor* Activator) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup", meta=(InvalidEnumValues="EWeaponSlot::None"))
	EWeaponSlot WeaponToGive = EWeaponSlot::SlugShot;

private:
	UPROPERTY()
	TWeakObjectPtr<UHDAPlayerWeaponManager> WeaponManager = nullptr;
};
