// 


#include "HDAPlayerWeaponManager.h"

#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponBase.h"

DEFINE_LOG_CATEGORY(LogPlayerWeaponManager);

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
#if WITH_EDITOR || !UE_BUILD_SHIPPING
		const FString SlotName = UHDAPlayerWeaponData::GetSlotName(WeaponSlot);
		const FString Message = FString::Printf(TEXT("Could NOT add weapon for %s slot. Player has it already."),
		                                        *SlotName);
		PrintLog(Message);
#endif
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

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FString SlotName = UHDAPlayerWeaponData::GetSlotName(WeaponSlot);
	const FString Message = FString::Printf(TEXT("%s weapon was added to %s slot."),
	                                        *NewWeapon->GetActorNameOrLabel(),
	                                        *SlotName);
	PrintLog(Message);
#endif
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

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FString PreviousSlotName = UHDAPlayerWeaponData::GetSlotName(PreviousWeaponSlot);
	const FString CurrentSlotName = UHDAPlayerWeaponData::GetSlotName(CurrentWeaponSlot);
	const FString Message = FString::Printf(TEXT("%s weapon was switched to %s weapon."),
	                                        *PreviousSlotName,
	                                        *CurrentSlotName);
	PrintLog(Message);
#endif
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

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FString AmmoName = UHDAPlayerWeaponData::GetAmmoTypeName(AmmoType);
	const FString Message = FString::Printf(TEXT("%s ammo was increased by %d. Current value %d/%d."),
	                                        *AmmoName,
	                                        Value,
	                                        Ammo.Value,
	                                        Ammo.MaxValue);
	PrintLog(Message);
#endif
}

AHDAPlayerWeaponBase* UHDAPlayerWeaponManager::GetCurrentWeapon() const
{
	return AcquiredWeapons[CurrentWeaponSlot];
}

FTrickyPropertyInt UHDAPlayerWeaponManager::GetCurrentAmmo() const
{
	return AmmoStash[CurrentAmmoType];
}

bool UHDAPlayerWeaponManager::HasEnoughCurrentAmmo() const
{
	return !GetCurrentAmmo().ReachedMinValue();
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

void UHDAPlayerWeaponManager::HandleWeaponShot(const AHDAPlayerWeaponBase* Weapon)
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
		StopShooting();
		//TODO: Deactivate weapon
	}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FString AmmoTypeName = UHDAPlayerWeaponData::GetAmmoTypeName(CurrentAmmoType);
	const FString Message = FString::Printf(TEXT("%s ammo was decreased by %d. Current value %d/%d."),
	                                        *AmmoTypeName,
	                                        CurrentShotCost,
	                                        Ammo.Value,
	                                        Ammo.MaxValue);
	PrintLog(Message);
#endif
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void UHDAPlayerWeaponManager::PrintLog(const FString& Message)
{
	UE_LOG(LogPlayerWeaponManager, Log, TEXT("%s"), *Message);
}
#endif
