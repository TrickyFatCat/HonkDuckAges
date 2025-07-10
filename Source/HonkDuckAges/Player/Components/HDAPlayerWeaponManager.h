// 

#pragma once

#include "CoreMinimal.h"
#include "TrickyPropertiesLibrary.h"
#include "Components/ActorComponent.h"
#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponData.h"
#include "HDAPlayerWeaponManager.generated.h"

class UCameraComponent;
class AHDAPlayerWeaponBase;

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerWeaponManager, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAmmoValueChangedDynamicSignature,
                                              UHDAPlayerWeaponManager*, Component,
                                              EWeaponAmmoType, AmmoType,
                                              const FTrickyPropertyInt&, Ammo,
                                              int32, DeltaValue);

UCLASS(ClassGroup=(Custom), PrioritizeCategories="WeaponManager")
class HONKDUCKAGES_API UHDAPlayerWeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UHDAPlayerWeaponManager();

protected:
	virtual void InitializeComponent() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnAmmoValueChangedDynamicSignature OnAmmoIncreased;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoValueChangedDynamicSignature OnAmmoDecreased;

	UFUNCTION()
	void StartShooting();

	UFUNCTION()
	void StopShooting();

	UFUNCTION()
	void AddWeapon(const EWeaponSlot WeaponSlot);

	UFUNCTION()
	bool HasWeapon(const EWeaponSlot WeaponSlot);

	UFUNCTION()
	void ChooseWeapon(const EWeaponSlot WeaponSlot);

	UFUNCTION()
	void ChooseWeaponByIndex(const int32 Index);

	UFUNCTION()
	void ChooseNextWeapon();

	UFUNCTION()
	void ChoosePreviousWeapon();

	UFUNCTION()
	void ChooseLastWeapon();

	UFUNCTION()
	void AddAmmo(const EWeaponAmmoType AmmoType, const int32 Value);

	UFUNCTION()
	void SubtractAmmo(const EWeaponAmmoType AmmoType, const int32 Value);

	UFUNCTION(BlueprintGetter)
	EWeaponSlot GetCurrentWeaponSlot() const { return CurrentWeaponSlot; }

	UFUNCTION(BlueprintGetter)
	EWeaponSlot GetPreviousWeaponSlot() const { return PreviousWeaponSlot; }

	UFUNCTION(BlueprintGetter)
	EWeaponAmmoType GetCurrentAmmoType() const { return CurrentAmmoType; }

	UFUNCTION(BlueprintGetter)
	int32 GetCurrentShotCost() const { return CurrentShotCost; }

	UFUNCTION(BlueprintPure)
	AHDAPlayerWeaponBase* GetCurrentWeapon() const;

	UFUNCTION(BlueprintPure)
	FTrickyPropertyInt GetCurrentAmmo() const;

	UFUNCTION(BlueprintPure)
	bool HasEnoughCurrentAmmo() const;

	UFUNCTION(BlueprintCallable)
	void GetAcquiredWeapons(TArray<AHDAPlayerWeaponBase*>& OutWeapons) const;

	UFUNCTION(BlueprintCallable)
	void GetAmmoStash(TMap<EWeaponAmmoType, FTrickyPropertyInt>& OutAmmoStash) const;

	UFUNCTION(BlueprintCallable)
	void GetAmmo(const EWeaponAmmoType AmmoType, FTrickyPropertyInt& OutAmmo) const;

protected:
	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> CameraComponent = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponManager")
	UHDAPlayerWeaponData* WeaponData = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetCurrentWeaponSlot, Category="WeaponManager")
	EWeaponSlot CurrentWeaponSlot = EWeaponSlot::None;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetCurrentAmmoType, Category="WeaponManager")
	EWeaponAmmoType CurrentAmmoType = EWeaponAmmoType::Gauge;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetCurrentShotCost, Category="WeaponManager")
	int32 CurrentShotCost = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetPreviousWeaponSlot, Category="WeaponManager")
	EWeaponSlot PreviousWeaponSlot = EWeaponSlot::None;

	UPROPERTY(VisibleInstanceOnly, Category="WeaponManager")
	TMap<EWeaponSlot, TObjectPtr<AHDAPlayerWeaponBase>> AcquiredWeapons{
		{EWeaponSlot::Shotgun, nullptr},
		{EWeaponSlot::SlugShot, nullptr},
		{EWeaponSlot::HeavyChainGun, nullptr},
		{EWeaponSlot::MiniRockets, nullptr},
		{EWeaponSlot::PlasmaBeam, nullptr},
		{EWeaponSlot::LaserCrossbow, nullptr},
		{EWeaponSlot::ShieldThrower, nullptr},
	};

	UPROPERTY(VisibleInstanceOnly, Category="WeaponManager")
	TMap<EWeaponAmmoType, FTrickyPropertyInt> AmmoStash{
		{EWeaponAmmoType::Gauge, FTrickyPropertyInt{}},
		{EWeaponAmmoType::Bullet, FTrickyPropertyInt{}},
		{EWeaponAmmoType::Energy, FTrickyPropertyInt{}},
		{EWeaponAmmoType::Shield, FTrickyPropertyInt{}}
	};

private:
	void InitAmmoStash();

	UFUNCTION()
	void HandleWeaponShot(AHDAPlayerWeaponBase* Weapon);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	static void PrintLog(const FString& Message);
#endif
};
