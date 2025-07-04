// 


#include "HDAPlayerWeaponManager.h"

#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponBase.h"


UHDAPlayerWeaponManager::UHDAPlayerWeaponManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UHDAPlayerWeaponManager::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		ensureMsgf(IsValid(WeaponData), TEXT("Invalid player weapon data"));

		if (IsValid(WeaponData))
		{
			for (auto& Slot : WeaponData->WeaponSlots)
			{
				ensureMsgf(IsValid(Slot.Value.WeaponClass), TEXT("%s | Invalid weapon class for %s slot"),
				           *WeaponData->GetName(),
				           *StaticEnum<EWeaponSlot>()->GetNameStringByValue(static_cast<int64>(Slot.Key)));
			}
		}

		InitAmmoStash();
		AddWeapon(EWeaponSlot::Shotgun);
		ChooseWeapon(EWeaponSlot::Shotgun);
		AddAmmo(EWeaponAmmoType::Gauge, 999);
	}
}

void UHDAPlayerWeaponManager::StartShooting()
{
	if (!HasWeapon(CurrentWeaponSlot))
	{
		return;
	}

	AHDAPlayerWeaponBase* CurrentWeapon = AcquiredWeapons[CurrentWeaponSlot];

	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	CurrentWeapon->StartShooting(FVector::ZeroVector);
}

void UHDAPlayerWeaponManager::StopShooting()
{
	if (!HasWeapon(CurrentWeaponSlot))
	{
		return;
	}

	AHDAPlayerWeaponBase* CurrentWeapon = AcquiredWeapons[CurrentWeaponSlot];

	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	CurrentWeapon->StopShooting();
}

void UHDAPlayerWeaponManager::AddWeapon(const EWeaponSlot WeaponSlot)
{
	if (HasWeapon(WeaponSlot))
	{
		return;
	}

	TSubclassOf<AHDAPlayerWeaponBase> WeaponClass = WeaponData->WeaponSlots[WeaponSlot].WeaponClass;

	if (!IsValid(WeaponClass))
	{
		return;
	}

	AHDAPlayerWeaponBase* NewWeapon = GetWorld()->SpawnActorDeferred<AHDAPlayerWeaponBase>(
		WeaponClass, FTransform::Identity);
	NewWeapon->SetOwner(GetOwner());
	NewWeapon->AttachToActor(GetOwner(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	NewWeapon->FinishSpawning(FTransform::Identity);
	AcquiredWeapons[WeaponSlot] = NewWeapon;
	ChooseWeapon(WeaponSlot);
}

bool UHDAPlayerWeaponManager::HasWeapon(const EWeaponSlot WeaponSlot)
{
	return IsValid(AcquiredWeapons[WeaponSlot]);
}

void UHDAPlayerWeaponManager::ChooseWeapon(const EWeaponSlot WeaponSlot)
{
	if (!IsValid(AcquiredWeapons[WeaponSlot]))
	{
		return;
	}

	if (PreviousWeaponSlot != CurrentWeaponSlot)
	{
		PreviousWeaponSlot = CurrentWeaponSlot;
		AHDAPlayerWeaponBase* PreviousWeapon = AcquiredWeapons[PreviousWeaponSlot];

		if (IsValid(PreviousWeapon))
		{
			PreviousWeapon->OnPlayerWeaponShot.RemoveDynamic(this, &UHDAPlayerWeaponManager::HandleWeaponShot);
		}
	}

	CurrentWeaponSlot = WeaponSlot;
	CurrentAmmoType = WeaponData->WeaponSlots[WeaponSlot].AmmoType;
	CurrentShotCost = WeaponData->WeaponSlots[WeaponSlot].ShotCost;

	AHDAPlayerWeaponBase* CurrentWeapon = AcquiredWeapons[CurrentWeaponSlot];

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->OnPlayerWeaponShot.AddUniqueDynamic(this, &UHDAPlayerWeaponManager::HandleWeaponShot);
	}
}

void UHDAPlayerWeaponManager::ChoosePreviousWeapon()
{
	ChooseWeapon(PreviousWeaponSlot);
}

void UHDAPlayerWeaponManager::AddAmmo(const EWeaponAmmoType AmmoType, const int32 Value)
{
	FTrickyPropertyInt& Ammo = AmmoStash[AmmoType];

	if (Ammo.ReachedMaxValue())
	{
		return;
	}

	Ammo.IncreaseValue(Value);
	Ammo.ClampToMax();
}

AHDAPlayerWeaponBase* UHDAPlayerWeaponManager::GetCurrentWeapon() const
{
	return AcquiredWeapons[CurrentWeaponSlot];
}

FTrickyPropertyInt UHDAPlayerWeaponManager::GetCurrentAmmo() const
{
	return AmmoStash[CurrentAmmoType];
}

void UHDAPlayerWeaponManager::GetAcquiredWeapons(TArray<AHDAPlayerWeaponBase*>& OutWeapons) const
{
	for (auto& WeaponSlot : AcquiredWeapons)
	{
		OutWeapons.AddUnique(WeaponSlot.Value);
	}
}

void UHDAPlayerWeaponManager::GetAmmoStash(TMap<EWeaponAmmoType, FTrickyPropertyInt>& OutAmmoStash) const
{
	OutAmmoStash = AmmoStash;
}

void UHDAPlayerWeaponManager::GetAmmo(const EWeaponAmmoType AmmoType, FTrickyPropertyInt& OutAmmo) const
{
	if (!AmmoStash.Contains(AmmoType))
	{
		return;
	}

	OutAmmo = AmmoStash[AmmoType];
}

void UHDAPlayerWeaponManager::InitAmmoStash()
{
	if (!ensureMsgf(!AmmoStash.IsEmpty(), TEXT("AmmoStash in PlayerManagerComponent is empty.")))
	{
		return;
	}

	for (auto& AmmoType : AmmoStash)
	{
		FTrickyPropertyInt& Ammo = AmmoType.Value;
		Ammo.MaxValue = WeaponData->DefaultWeaponAmmo.FindChecked(AmmoType.Key);
		Ammo.Value = 0;
	}
}

void UHDAPlayerWeaponManager::HandleWeaponShot(AHDAPlayerWeaponBase* Weapon)
{
	if (!IsValid(Weapon))
	{
		return;
	}

	FTrickyPropertyInt& Ammo = AmmoStash[CurrentAmmoType];
	Ammo.DecreaseValue(CurrentShotCost);
	Ammo.ClampToMin();

	if (Ammo.ReachedMinValue())
	{
		//TODO: Deactivate weapon
	}
}
