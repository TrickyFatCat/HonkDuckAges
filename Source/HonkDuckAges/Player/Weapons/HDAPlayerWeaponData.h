// 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HonkDuckAges/Player/Components/HDAPlayerMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "HDAPlayerWeaponData.generated.h"

class UHDAPlayerMovementComponent;
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
	None
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

USTRUCT(BlueprintType)
struct FSwitchingAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="SwitchingAnimation",
		meta=(ClampMin=0, UIMin=0, Delta=0.1, ForceUnits="Seconds"))
	float AnimationDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SwitchingAnimation")
	FVector HideLocation = FVector(0.f, 24.f, -100.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SwitchingAnimation")
	FRotator HideRotation = FRotator(-45.f, 0.f, 0.f);

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="SwitchingAnimation",
		meta=(ClampMin=0, UIMin=0, Delta=1))
	float AnimationExponent = 2.f;

	float CurrentAnimationDuration = 0.f;

	float GetNormalizedDuration() const
	{
		return CurrentAnimationDuration / AnimationDuration;
	}
};

USTRUCT(BlueprintType)
struct FRotationSwayData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Degrees"))
	FRotator Threshold = FRotator(10.f, 10.f, 1.f);

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway")
	FVector Power = FVector(2.f, 3.f, 2.f);

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway",
		meta=(ClampMin=0, UIMin=0, Delta=1))
	float Speed = 5.f;

	FRotator TargetRotation = FRotator::ZeroRotator;

	void CalculateTargetRotation(const FVector2D& Value)
	{
		TargetRotation.Roll = Value.X * Power.X;
		TargetRotation.Pitch = -Value.Y * Power.Y;
		TargetRotation.Yaw = Value.X * Power.Z;
	}
};

USTRUCT(BlueprintType)
struct FLocationSwayData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway",
		meta=(ForceUnits="Centimeters"))
	FVector Threshold = FVector(3.f, 4.f, 5.f);

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway",
		meta=(ClampMin=1, UIMin=1, ClampMax=100, UIMax=100, Delta=1))
	float Speed = 10.f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway",
		meta=(ClampMin=0, UIMin=0))
	float Frequency = 8.f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway",
		meta=(ForceUnits="Centimeters"))
	FVector Amplitude = FVector(0.f, 1.f, 0.5f);

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="WeaponSway",
		meta=(ClampMin=0, UIMin=0, ClampMax=1, UIMax=1, Delta=0.1))
	float DeadZone = 0.1f;

	FVector CurrentLateralOffset = FVector::ZeroVector;

	float CurrentVerticalOffset = 0.f;

	UPROPERTY()
	TWeakObjectPtr<UHDAPlayerMovementComponent> PlayerMovementComponent = nullptr;

	void InterpolateLateralOffset(const FVector& TargetLateralOffset, const float DeltaTime);

	void InterpolateVerticalOffset(const float TargetVerticalOffset, const float DeltaTime);

	void CalculateLocationOffset(const float DeltaTime, FVector& OutLocation);

private:
	float CheckDeadZone(const float Value) const;
};

/**
 * 
 */
UCLASS()
class HONKDUCKAGES_API UHDAPlayerWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WeaponData")
	FVector WeaponSpawnPosition = FVector(0, 24.f, -24.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	FSwitchingAnimationData SwitchingAnimationData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	FRotationSwayData RotationSwayData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	FLocationSwayData LocationSwayData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WeaponManager", meta=(InvalidEnumValues="None"))
	EWeaponSlot DefaultWeaponSlot = EWeaponSlot::Shotgun;

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
