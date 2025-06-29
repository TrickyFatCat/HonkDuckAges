// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDAPlayerWeaponManager.generated.h"

class AHDAPlayerWeaponBase;

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Shotgun,
	SlugShot,
	HeavyChainGun,
	MiniRockets,
	PlasmaBeam,
	LaserCrossbow,
	ShieldThrower,
};

UCLASS(ClassGroup=(Custom), PrioritizeCategories="WeaponManager")
class HONKDUCKAGES_API UHDAPlayerWeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UHDAPlayerWeaponManager();

protected:
	virtual void InitializeComponent() override;

public:
	UFUNCTION()
	void StartShooting();

	UFUNCTION()
	void StopShooting();

	UFUNCTION()
	void AddWeapon(const EWeaponSlot WeaponSlot);

	UFUNCTION()
	void ChooseWeapon(const EWeaponSlot WeaponSlot);

	UFUNCTION()
	void ChoosePreviousWeapon();

	UFUNCTION(BlueprintGetter)
	EWeaponSlot GetCurrentWeaponSlot() const { return CurrentWeaponSlot; }

	UFUNCTION(BlueprintGetter)
	EWeaponSlot GetPreviousWeaponSlot() const { return PreviousWeaponSlot; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponManager", meta=(ReadOnlyKeys, ForceInlineRow))
	TMap<EWeaponSlot, TSubclassOf<AHDAPlayerWeaponBase>> WeaponSlots{
		{EWeaponSlot::Shotgun, nullptr},
		{EWeaponSlot::SlugShot, nullptr},
		{EWeaponSlot::HeavyChainGun, nullptr},
		{EWeaponSlot::MiniRockets, nullptr},
		{EWeaponSlot::PlasmaBeam, nullptr},
		{EWeaponSlot::LaserCrossbow, nullptr},
		{EWeaponSlot::ShieldThrower, nullptr},
	};

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetCurrentWeaponSlot, Category="WeaponManager")
	EWeaponSlot CurrentWeaponSlot = EWeaponSlot::Shotgun;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetPreviousWeaponSlot, Category="WeaponManager")
	EWeaponSlot PreviousWeaponSlot = EWeaponSlot::Shotgun;

	UPROPERTY(VisibleInstanceOnly, Category="WeaponManager")
	TMap<EWeaponSlot, TObjectPtr<AHDAPlayerWeaponBase>> AcquiredWeapons{};
};
