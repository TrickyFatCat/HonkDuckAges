// 


#include "HDAPlayerWeaponManager.h"

#include "HDAPlayerMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponBase.h"
#include "Kismet/KismetMathLibrary.h"

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

			WeaponSpawnLocation = WeaponData->WeaponSpawnPosition;
			SwitchingAnimationData = WeaponData->SwitchingAnimationData;
			RotationSwayData = WeaponData->RotationSwayData;
			LocationSwayData = WeaponData->LocationSwayData;
			BreathingAnimationData = WeaponData->BreathAnimationData;
		}

		CameraComponent = GetOwner()->GetComponentByClass<UCameraComponent>();
		LocationSwayData.PlayerMovementComponent = GetOwner()->FindComponentByClass<UHDAPlayerMovementComponent>();
		InitAmmoStash();
		const EWeaponSlot DefaultWeaponSlot = IsValid(WeaponData)
			                                      ? WeaponData->DefaultWeaponSlot
			                                      : EWeaponSlot::Shotgun;
		AddWeapon(DefaultWeaponSlot);
		ChooseWeapon(DefaultWeaponSlot);
		AddAmmo(CurrentAmmoType, 999);
	}
}

void UHDAPlayerWeaponManager::TickComponent(float DeltaTime,
                                            ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AnimateRotationSway(DeltaTime);
	AnimateLocationSway(DeltaTime);
	PlaySwitchAnimation(DeltaTime);
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

	CurrentWeapon->StartShooting();
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

	const FWeaponData& NewWeaponData = WeaponData->WeaponSlots[WeaponSlot];
	TSubclassOf<AHDAPlayerWeaponBase> WeaponClass = NewWeaponData.WeaponClass;

	if (!IsValid(WeaponClass))
	{
		return;
	}

	FTransform AttachmentTransform = FTransform::Identity;
	AttachmentTransform.SetLocation(SwitchingAnimationData.HideLocation);
	AttachmentTransform.SetRotation(SwitchingAnimationData.HideRotation.Quaternion());

	AHDAPlayerWeaponBase* NewWeapon = GetWorld()->SpawnActorDeferred<AHDAPlayerWeaponBase>(
		WeaponClass, AttachmentTransform);
	NewWeapon->SetOwner(GetOwner());
	NewWeapon->SetOwningWeaponManager(this);
	NewWeapon->FinishSpawning(AttachmentTransform);

	if (CameraComponent.IsValid())
	{
		NewWeapon->AttachToComponent(CameraComponent.Get(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	else
	{
		NewWeapon->AttachToActor(GetOwner(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	}

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
	if (AcquiredWeapons.IsEmpty())
	{
		return false;
	}

	return IsValid(AcquiredWeapons[WeaponSlot]);
}

void UHDAPlayerWeaponManager::ChooseWeapon(const EWeaponSlot WeaponSlot)
{
	StartSwitchingWeapon(WeaponSlot);
}

void UHDAPlayerWeaponManager::ChooseWeaponByIndex(const int32 Index)
{
	TArray<EWeaponSlot> Slots;
	AcquiredWeapons.GetKeys(Slots);

	if (Slots.IsEmpty() || !Slots.IsValidIndex(Index))
	{
		return;
	}

	if (!IsValid(AcquiredWeapons[Slots[Index]]))
	{
		return;
	}

	ChooseWeapon(Slots[Index]);
}

void UHDAPlayerWeaponManager::ChooseNextWeapon()
{
	TArray<AHDAPlayerWeaponBase*> Weapons;
	GetAcquiredWeapons(Weapons);

	if (Weapons.Num() <= 1)
	{
		return;
	}

	int32 Index = Weapons.IndexOfByKey(AcquiredWeapons[CurrentWeaponSlot]);
	Index += 1;

	if (!Weapons.IsValidIndex(Index))
	{
		return;
	}

	ChooseWeapon(*AcquiredWeapons.FindKey(Weapons[Index]));
}

void UHDAPlayerWeaponManager::ChoosePreviousWeapon()
{
	TArray<AHDAPlayerWeaponBase*> Weapons;
	GetAcquiredWeapons(Weapons);

	if (Weapons.Num() <= 1)
	{
		return;
	}

	int32 Index = Weapons.IndexOfByKey(AcquiredWeapons[CurrentWeaponSlot]);
	Index -= 1;

	if (!Weapons.IsValidIndex(Index))
	{
		return;
	}

	ChooseWeapon(*AcquiredWeapons.FindKey(Weapons[Index]));
}

void UHDAPlayerWeaponManager::ChooseLastWeapon()
{
	ChooseWeapon(PreviousWeaponSlot);
}

void UHDAPlayerWeaponManager::AddAmmo(const EWeaponAmmoType AmmoType, const int32 Value)
{
	if (Value <= 0)
	{
		return;
	}

	FTrickyPropertyInt& Ammo = AmmoStash[AmmoType];

	if (Ammo.ReachedMaxValue())
	{
		return;
	}

	Ammo.IncreaseValue(Value);
	Ammo.ClampToMax();
	OnAmmoIncreased.Broadcast(this, AmmoType, Ammo, Value);

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

void UHDAPlayerWeaponManager::SubtractAmmo(const EWeaponAmmoType AmmoType, const int32 Value)
{
	if (Value <= 0 || bHasInfiniteAmmo)
	{
		return;
	}

	FTrickyPropertyInt& Ammo = AmmoStash[AmmoType];

	if (Ammo.ReachedMinValue())
	{
		return;
	}

	Ammo.DecreaseValue(Value);
	Ammo.ClampToMin();
	OnAmmoDecreased.Broadcast(this, AmmoType, Ammo, Value);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FString AmmoTypeName = UHDAPlayerWeaponData::GetAmmoTypeName(CurrentAmmoType);
	const FString Message = FString::Printf(TEXT("%s ammo was decreased by %d. Current value %d/%d."),
	                                        *AmmoTypeName,
	                                        CurrentShotCost,
	                                        Ammo.Value,
	                                        Ammo.MaxValue);
	PrintLog(Message);
#endif

	if (CurrentAmmoType == AmmoType && Ammo.ReachedMinValue())
	{
		StopShooting();
	}
}

void UHDAPlayerWeaponManager::SetHasInfiniteAmmo(const bool Value)
{
	if (bHasInfiniteAmmo == Value)
	{
		return;
	}

	bHasInfiniteAmmo = Value;

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FString BoolValue = bHasInfiniteAmmo ? TEXT("TRUE") : TEXT("FALSE");
	const FString Message = FString::Printf(TEXT("Infinite ammo was set to %s."), *BoolValue);
	PrintLog(Message);
#endif
}

AHDAPlayerWeaponBase* UHDAPlayerWeaponManager::GetCurrentWeapon() const
{
	if (CurrentWeaponSlot == EWeaponSlot::None)
	{
		return nullptr;
	}

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
	for (auto& [WeaponSlot, Weapon] : AcquiredWeapons)
	{
		if (!IsValid(Weapon))
		{
			continue;
		}

		OutWeapons.AddUnique(Weapon);
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

EWeaponAmmoType UHDAPlayerWeaponManager::GetAmmoTypeForSlot(const EWeaponSlot WeaponSlot) const
{
	return WeaponData->WeaponSlots[WeaponSlot].AmmoType;
}

void UHDAPlayerWeaponManager::CalculateTargetSwayRotation(const FVector2D& Value)
{
	RotationSwayData.CalculateTargetRotation(Value);
}

void UHDAPlayerWeaponManager::AnimateRotationSway(const float DeltaTime) const
{
	AHDAPlayerWeaponBase* CurrentWeapon = GetCurrentWeapon();

	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	FRotator CurrentRotation = CurrentWeapon->GetRootComponent()->GetRelativeRotation();
	RotationSwayData.InterpolateCurrentRotation(DeltaTime, CurrentRotation);
	CurrentWeapon->SetActorRelativeRotation(CurrentRotation);
}

void UHDAPlayerWeaponManager::AnimateLocationSway(const float DeltaTime)
{
	FVector WeaponOffset = FVector::ZeroVector;
	LocationSwayData.CalculateLocationOffset(DeltaTime, WeaponOffset);

	FVector BreathingOffset = FVector::ZeroVector;
	if (LocationSwayData.PlayerMovementComponent->Velocity.IsNearlyZero()
		&& GetCurrentWeapon()->GetCurrentState() != EWeaponState::Shooting)
	{
		BreathingAnimationData.CalculateLocationOffset(GetWorld()->GetTimeSeconds(), BreathingOffset);
	}

	AHDAPlayerWeaponBase* CurrentWeapon = GetCurrentWeapon();

	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	const FVector CurrentLocation = CurrentWeapon->GetRootComponent()->GetRelativeLocation();
	const FVector TargetLocation = WeaponSpawnLocation + WeaponOffset + BreathingOffset;
	const FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, LocationSwayData.Speed);

	CurrentWeapon->SetActorRelativeLocation(NewLocation);
}

void UHDAPlayerWeaponManager::HideCurrentWeapon()
{
	bIsHiding = true;
	SwitchingAnimationData.CurrentAnimationDuration = SwitchingAnimationData.AnimationDuration;
}

void UHDAPlayerWeaponManager::ShowCurrentWeapon()
{
	bIsHiding = false;
	SwitchingAnimationData.CurrentAnimationDuration = SwitchingAnimationData.AnimationDuration;
}

void UHDAPlayerWeaponManager::PlaySwitchAnimation(const float DeltaTime)
{
	if (!IsValid(GetCurrentWeapon()) || SwitchingAnimationData.CurrentAnimationDuration <= 0.f)
	{
		return;
	}

	SwitchingAnimationData.CurrentAnimationDuration -= DeltaTime;
	const float NormalizedDuration = SwitchingAnimationData.GetNormalizedDuration();
	const float Progress = FMath::Abs(1.f * static_cast<int32>(!bIsHiding) - NormalizedDuration);
	const FVector NewLocation = FMath::InterpEaseInOut(SwitchingAnimationData.HideLocation,
	                                                   WeaponSpawnLocation,
	                                                   Progress,
	                                                   SwitchingAnimationData.AnimationExponent);
	const FRotator NewRotator = FMath::InterpEaseInOut(SwitchingAnimationData.HideRotation,
	                                                   FRotator::ZeroRotator,
	                                                   Progress,
	                                                   SwitchingAnimationData.AnimationExponent);
	GetCurrentWeapon()->SetActorRelativeLocation(NewLocation);
	GetCurrentWeapon()->SetActorRelativeRotation(NewRotator);

	if (SwitchingAnimationData.CurrentAnimationDuration > 0.f)
	{
		return;
	}

	if (bIsHiding)
	{
		GetCurrentWeapon()->SetActorHiddenInGame(true);
		FinishSwitchingWeapon();
	}
	else
	{
		GetCurrentWeapon()->ActivateWeapon();
	}
}

void UHDAPlayerWeaponManager::StartSwitchingWeapon(const EWeaponSlot WeaponSlot)
{
	if (!HasWeapon(WeaponSlot) || CurrentWeaponSlot == WeaponSlot)
	{
		return;
	}

	TargetWeaponSlot = WeaponSlot;

	if (SwitchingAnimationData.CurrentAnimationDuration > 0.f)
	{
		return;
	}

	if (PreviousWeaponSlot == CurrentWeaponSlot)
	{
		FinishSwitchingWeapon();
	}
	else
	{
		PreviousWeaponSlot = CurrentWeaponSlot;
		AHDAPlayerWeaponBase* PreviousWeapon = AcquiredWeapons[PreviousWeaponSlot];

		if (IsValid(PreviousWeapon))
		{
			PreviousWeapon->OnPlayerWeaponShot.RemoveDynamic(this, &UHDAPlayerWeaponManager::HandleWeaponShot);
			PreviousWeapon->DeactivateWeapon();
		}

		HideCurrentWeapon();
	}
}

void UHDAPlayerWeaponManager::FinishSwitchingWeapon()
{
	CurrentWeaponSlot = TargetWeaponSlot;
	CurrentAmmoType = WeaponData->WeaponSlots[TargetWeaponSlot].AmmoType;
	CurrentShotCost = WeaponData->WeaponSlots[TargetWeaponSlot].ShotCost;

	AHDAPlayerWeaponBase* CurrentWeapon = AcquiredWeapons[CurrentWeaponSlot];

	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->OnPlayerWeaponShot.AddUniqueDynamic(this, &UHDAPlayerWeaponManager::HandleWeaponShot);
		CurrentWeapon->SetActorHiddenInGame(false);
	}

	ShowCurrentWeapon();

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	const FString PreviousSlotName = UHDAPlayerWeaponData::GetSlotName(PreviousWeaponSlot);
	const FString CurrentSlotName = UHDAPlayerWeaponData::GetSlotName(CurrentWeaponSlot);
	const FString Message = FString::Printf(TEXT("%s weapon was switched to %s weapon."),
	                                        *PreviousSlotName,
	                                        *CurrentSlotName);
	PrintLog(Message);
#endif
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
		Ammo.MaxValue = IsValid(WeaponData)
			                ? WeaponData->DefaultWeaponAmmo.FindChecked(AmmoType.Key)
			                : 0;
		Ammo.Value = 0;
	}
}

void UHDAPlayerWeaponManager::HandleWeaponShot(AHDAPlayerWeaponBase* Weapon)
{
	SubtractAmmo(CurrentAmmoType, CurrentShotCost);
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void UHDAPlayerWeaponManager::PrintLog(const FString& Message)
{
	UE_LOG(LogPlayerWeaponManager, Log, TEXT("%s"), *Message);
}
#endif
