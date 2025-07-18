// 


#include "HDAPlayerWeaponAnimComponent.h"

#include "HDAPlayerWeaponBase.h"
#include "Kismet/KismetMathLibrary.h"


UHDAPlayerWeaponAnimComponent::UHDAPlayerWeaponAnimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}


void UHDAPlayerWeaponAnimComponent::TickComponent(float DeltaTime,
                                                  ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector ShakeOffset = FVector::ZeroVector;
	FRotator ShakeRotation = FRotator::ZeroRotator;

	CalculateShakeOffset(DeltaTime, ShakeOffset, ShakeRotation);

	FVector RecoilOffset = FVector::ZeroVector;
	FRotator RecoilRotation = FRotator::ZeroRotator;

	CalculateRecoilOffset(DeltaTime, RecoilOffset, RecoilRotation);

	const FRotator RotationOffset = UKismetMathLibrary::ComposeRotators(ShakeRotation, RecoilRotation);

	const FVector NewLocation = InitialLocation + ShakeOffset + RecoilOffset;
	const FRotator NewRotation = UKismetMathLibrary::ComposeRotators(InitialRotation, RotationOffset);
	SetRelativeLocationAndRotation(NewLocation, NewRotation);
}

void UHDAPlayerWeaponAnimComponent::InitializeComponent()
{
	Super::InitializeComponent();

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		InitialLocation = GetRelativeLocation();
		InitialRotation = GetRelativeRotation();
		OwningWeapon = Cast<AHDAPlayerWeaponBase>(GetOwner());

		if (OwningWeapon.IsValid())
		{
			OwningWeapon->OnPlayerWeaponShot.AddUniqueDynamic(this, &UHDAPlayerWeaponAnimComponent::HandleWeaponShot);
		}
	}
}

void UHDAPlayerWeaponAnimComponent::AddShake(const float Value)
{
	if (Value <= 0.f)
	{
		return;
	}

	ShakeStrength += Value;
	ShakeStrength = FMath::Clamp(ShakeStrength, 0.f, 1.f);
}

void UHDAPlayerWeaponAnimComponent::CalculateShakeOffset(const float DeltaTime,
                                                         FVector& OutLocation,
                                                         FRotator& OutRotation)
{
	if (ShakeStrength <= 0.f)
	{
		OutLocation = FVector::ZeroVector;
		OutRotation = FRotator::ZeroRotator;
		return;
	}

	const float CurrentShakeFadeSpeed = OwningWeapon->GetCurrentState() == EWeaponState::Shooting
		                                    ? ShakeFadeSpeed
		                                    : ShakeFadeSpeedOnStopShooting;
	ShakeStrength = FMath::FInterpTo(ShakeStrength, 0.f, DeltaTime, CurrentShakeFadeSpeed);
	const float ShakeProgress = IsValid(ShakeStrengthCurve)
		                            ? ShakeStrengthCurve->GetFloatValue(1 - ShakeStrength)
		                            : ShakeStrength;

	OutLocation = ShakeLocationThreshold * FMath::VRand() * ShakeProgress;

	OutRotation.Roll = ShakeRotationThreshold.Roll * FMath::RandRange(-1.f, 1.f) * ShakeProgress;
	OutRotation.Pitch = ShakeRotationThreshold.Pitch * FMath::RandRange(-1.f, 1.f) * ShakeProgress;
	OutRotation.Yaw = ShakeRotationThreshold.Yaw * FMath::RandRange(-1.f, 1.f) * ShakeProgress;
}

float UHDAPlayerWeaponAnimComponent::GetNormalizedRemainingTime() const
{
	if (!OwningWeapon.IsValid())
	{
		return -1.f;
	}

	return OwningWeapon->GetNormalizedRemainingShotTime();
}

float UHDAPlayerWeaponAnimComponent::GetNormalizedElapsedTime() const
{
	if (!OwningWeapon.IsValid())
	{
		return -1.f;
	}

	return OwningWeapon->GetNormalizedElapsedShotTime();
}

void UHDAPlayerWeaponAnimComponent::CalculateRecoilOffset(const float DeltaTime,
                                                          FVector& OutLocation,
                                                          FRotator& OutRotation)
{
	if (!OwningWeapon.IsValid())
	{
		OutLocation = FVector::ZeroVector;
		OutRotation = FRotator::ZeroRotator;
		return;
	}

	const float ShotTimerProgress = GetNormalizedRemainingTime();

	switch (OwningWeapon->GetWeaponMode())
	{
	case EWeaponMode::FullAuto:
		if (!bRecoilEachShotOnFullAuto)
		{
			RecoilTargetStrength = OwningWeapon->GetCurrentState() == EWeaponState::Shooting ? 1.f : 0.f;
		}
		else
		{
			RecoilTargetStrength = ShotTimerProgress <= RecoilShotDelayThreshold ? 0.f : 1.f;
		}
		break;
	case EWeaponMode::SemiAuto:
		RecoilTargetStrength = ShotTimerProgress <= RecoilShotDelayThreshold ? 0.f : 1.f;
		break;
	}


	const float RecoilFadeFactor = IsValid(RecoilFadeCurve)
		                               ? RecoilFadeCurve->GetFloatValue(1 - ShotTimerProgress)
		                               : 1.f;

	RecoilStrength = FMath::FInterpTo(RecoilStrength,
	                                  RecoilTargetStrength,
	                                  DeltaTime,
	                                  RecoilFadeSpeed * RecoilFadeFactor);

	const float RecoilProgress = IsValid(RecoilStrengthCurve)
		                             ? RecoilStrengthCurve->GetFloatValue(RecoilStrength)
		                             : RecoilStrength;

	OutLocation = RecoilLocationThreshold * RecoilProgress;
	OutRotation = RecoilRotationThreshold * RecoilProgress;
}

void UHDAPlayerWeaponAnimComponent::HandleWeaponShot(AHDAPlayerWeaponBase* Weapon)
{
	AddShake(ShakePerShot);
}
