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
	
	AHDAPlayerWeaponBase* NewWeapon = GetWorld()->SpawnActorDeferred<AHDAPlayerWeaponBase>(WeaponClass, FTransform::Identity);
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

	PreviousWeaponSlot = CurrentWeaponSlot;
	CurrentWeaponSlot = WeaponSlot;
	CurrentAmmoType = WeaponData->WeaponSlots[WeaponSlot].AmmoType;
	CurrentShotCost = WeaponData->WeaponSlots[WeaponSlot].ShotCost;
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
