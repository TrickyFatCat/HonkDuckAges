// 


#include "HDAPlayerWeaponData.h"

void FRotationSwayData::CalculateTargetRotation(const FVector2D& Value)
{
	TargetRotation.Roll = Value.X * Power.X;
	TargetRotation.Pitch = -Value.Y * Power.Y;
	TargetRotation.Yaw = Value.X * Power.Z;
}

void FRotationSwayData::InterpolateCurrentRotation(const float DeltaTime, FRotator& CurrentRotation) const
{
	CurrentRotation = FMath::RInterpTo(CurrentRotation,
	                                   TargetRotation,
	                                   DeltaTime,
	                                   Speed);

	CurrentRotation.Roll = FMath::Clamp(CurrentRotation.Roll,
	                                    -Threshold.Roll,
	                                    Threshold.Roll);
	CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch,
	                                     -Threshold.Pitch,
	                                     Threshold.Pitch);
	CurrentRotation.Yaw = FMath::Clamp(CurrentRotation.Yaw,
	                                   -Threshold.Yaw,
	                                   Threshold.Yaw);
}

void FLocationSwayData::InterpolateLateralOffset(const FVector& TargetLateralOffset, const float DeltaTime)
{
	CurrentLateralOffset = FMath::VInterpTo(CurrentLateralOffset,
	                                        TargetLateralOffset,
	                                        DeltaTime,
	                                        Speed);

	CurrentLateralOffset.X = CheckDeadZone(CurrentLateralOffset.X);
	CurrentLateralOffset.Y = CheckDeadZone(CurrentLateralOffset.Y);
}

void FLocationSwayData::InterpolateVerticalOffset(const float TargetVerticalOffset, const float DeltaTime)
{
	CurrentVerticalOffset = FMath::FInterpTo(CurrentVerticalOffset,
	                                         TargetVerticalOffset,
	                                         DeltaTime,
	                                         Speed);

	CurrentVerticalOffset = CheckDeadZone(CurrentVerticalOffset);
}

void FLocationSwayData::CalculateLocationOffset(const float DeltaTime, FVector& OutLocation)
{
	if (!PlayerMovementComponent.IsValid())
	{
		OutLocation = FVector::ZeroVector;
		return;
	}

	const AActor* PlayerActor = PlayerMovementComponent->GetOwner();

	if (!IsValid(PlayerActor))
	{
		OutLocation = FVector::ZeroVector;
		return;
	}

	FVector TargetLateralOffset = UKismetMathLibrary::Quat_UnrotateVector(
		PlayerActor->GetActorRotation().Quaternion(),
		PlayerMovementComponent->GetLateralVelocity());

	TargetLateralOffset = TargetLateralOffset / PlayerMovementComponent->MaxWalkSpeed;
	TargetLateralOffset.X = FMath::Clamp(TargetLateralOffset.X, -1.f, 1.f);
	TargetLateralOffset.Y = FMath::Clamp(TargetLateralOffset.Y, -1.f, 1.f);

	InterpolateLateralOffset(TargetLateralOffset, DeltaTime);
	OutLocation = CurrentLateralOffset * Threshold;

	float TargetVerticalOffset = PlayerMovementComponent->GetNormalizedVerticalSpeed();
	TargetVerticalOffset = FMath::Clamp(TargetVerticalOffset, -1.0f, 1.0f);
	InterpolateVerticalOffset(TargetVerticalOffset, DeltaTime);
	OutLocation.Z += CurrentVerticalOffset * Threshold.Z;

	const float NormalizedLateralSpeed = PlayerMovementComponent->GetNormalizedLateralSpeed();

	if (!PlayerMovementComponent->IsFalling() && NormalizedLateralSpeed >= 1.f)
	{
		const float Time = PlayerActor->GetWorld()->GetTimeSeconds();
		const float MovementSin = FMath::Sin(NormalizedLateralSpeed * Frequency * Time);
		const float MovementSinSquared = MovementSin * MovementSin;

		OutLocation.X += Amplitude.X * MovementSinSquared;
		OutLocation.Y += Amplitude.Y * MovementSin;
		OutLocation.Z += Amplitude.Z * MovementSinSquared;
	}
}

float FLocationSwayData::CheckDeadZone(const float Value) const
{
	return FMath::Abs(Value) >= DeadZone ? Value : 0.f;
}

FString UHDAPlayerWeaponData::GetSlotName(const EWeaponSlot WeaponSlot)
{
	return StaticEnum<EWeaponSlot>()->GetNameStringByValue(static_cast<int64>(WeaponSlot));
}

FString UHDAPlayerWeaponData::GetAmmoTypeName(const EWeaponAmmoType AmmoType)
{
	return StaticEnum<EWeaponAmmoType>()->GetNameStringByValue(static_cast<int64>(AmmoType));
}
