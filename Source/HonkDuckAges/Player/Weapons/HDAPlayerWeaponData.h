// 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HDAPlayerWeaponData.generated.h"

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

UENUM(BlueprintType)
enum class EWeaponAmmoType : uint8
{
	Gauge,
	Bullet,
	Energy,
	Shield
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WeaponData")
	TSubclassOf<AHDAPlayerWeaponBase> WeaponClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WeaponData")
	EWeaponAmmoType AmmoType = EWeaponAmmoType::Gauge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="WeaponData", meta=(ClampMin=1, UIMin=1, Delta=1))
	int32 ShotCost = 1;
};

/**
 * 
 */
UCLASS()
class HONKDUCKAGES_API UHDAPlayerWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponManager", meta=(ReadOnlyKeys, ForceInlineRow))
	TMap<EWeaponSlot, FWeaponData> WeaponSlots{
		{EWeaponSlot::Shotgun, FWeaponData{nullptr, EWeaponAmmoType::Gauge, 2}},
		{EWeaponSlot::SlugShot, FWeaponData{nullptr, EWeaponAmmoType::Gauge, 1}},
		{EWeaponSlot::HeavyChainGun, FWeaponData{nullptr, EWeaponAmmoType::Bullet, 1}},
		{EWeaponSlot::MiniRockets, FWeaponData{nullptr, EWeaponAmmoType::Bullet, 1}},
		{EWeaponSlot::PlasmaBeam, FWeaponData{nullptr, EWeaponAmmoType::Energy, 1}},
		{EWeaponSlot::LaserCrossbow, FWeaponData{nullptr, EWeaponAmmoType::Energy, 1}},
		{EWeaponSlot::ShieldThrower, FWeaponData{nullptr, EWeaponAmmoType::Shield, 1}},
	};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponManager", meta=(ReadOnlyKeys, ForceInlineRow))
	TMap<EWeaponAmmoType, int32> DefaultWeaponAmmo{
		{EWeaponAmmoType::Gauge, 40},
		{EWeaponAmmoType::Bullet, 100},
		{EWeaponAmmoType::Energy, 200},
		{EWeaponAmmoType::Shield, 20}
	};

	UFUNCTION()
	static FString GetSlotName(const EWeaponSlot WeaponSlot);

	UFUNCTION()
	static FString GetAmmoTypeName(const EWeaponAmmoType AmmoType);
};
