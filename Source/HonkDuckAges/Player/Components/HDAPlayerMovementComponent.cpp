// 


#include "HDAPlayerMovementComponent.h"


UHDAPlayerMovementComponent::UHDAPlayerMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHDAPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UHDAPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHDAPlayerMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName JumpHeightName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpHeight);
	const FName GravityScaleName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, GravityScale);

	if (PropertyName == JumpHeightName || PropertyName == GravityScaleName)
	{
		JumpZVelocity = CalculateJumpZVelocity();
	}

	const FName JumpZVelocityName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpZVelocity);

	if (PropertyName == JumpZVelocityName)
	{
		JumpHeight = GetMaxJumpHeight();
	}

	const FName DashDurationName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, DashDuration);
	const FName DashDistanceName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, DashDistance);

	if (PropertyName == DashDurationName || PropertyName == DashDistanceName)
	{
		DashSpeed = DashDistance / DashDuration;
	}
}

bool UHDAPlayerMovementComponent::CanEverJump() const
{
	return Super::CanEverJump() && !bIsDashing;
}

void UHDAPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);

	bCanDash = !IsDashOnCooldown();
}

void UHDAPlayerMovementComponent::StartDashing()
{
	if (bIsDashing || IsDashOnCooldown() || !bCanDash)
	{
		return;
	}

	bIsDashing = true;
	bCanDash = false;

	CalculateDashVelocity(Velocity);

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(DashDurationTimer,
	                      this,
	                      &UHDAPlayerMovementComponent::FinishDashing,
	                      DashDuration,
	                      false);
	TimerManager.SetTimer(DashCooldownTimer,
	                      this,
	                      &UHDAPlayerMovementComponent::HandleDashCooldownFinished,
	                      DashCooldownDuration,
	                      false);
}

void UHDAPlayerMovementComponent::FinishDashing()
{
	if (!bIsDashing)
	{
		return;
	}

	bIsDashing = false;
	bCanDash = !IsDashOnCooldown() && !IsFalling();
}

void UHDAPlayerMovementComponent::HandleDashCooldownFinished()
{
	bCanDash = !IsFalling();
}

float UHDAPlayerMovementComponent::GetDashCooldownElapsedTime() const
{
	if (!GetWorld()->IsGameWorld())
	{
		return -1.f;
	}

	const FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (!TimerManager.IsTimerActive(DashCooldownTimer))
	{
		return -1.f;
	}

	return TimerManager.GetTimerElapsed(DashCooldownTimer);
}

float UHDAPlayerMovementComponent::GetDashCooldownRemainingTime() const
{
	if (!GetWorld()->IsGameWorld())
	{
		return -1.f;
	}

	const FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (!TimerManager.IsTimerActive(DashCooldownTimer))
	{
		return -1.f;
	}

	return TimerManager.GetTimerRemaining(DashCooldownTimer);
}

bool UHDAPlayerMovementComponent::IsDashOnCooldown() const
{
	if (!GetWorld()->IsGameWorld())
	{
		return false;
	}

	const FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	return TimerManager.IsTimerActive(DashCooldownTimer);
}

void UHDAPlayerMovementComponent::SetCanDash(const bool Value)
{
	if (bCanDash == Value)
	{
		return;
	}

	bCanDash = Value;
}

float UHDAPlayerMovementComponent::CalculateJumpZVelocity() const
{
	return FMath::Sqrt(-2 * GetGravityZ() * JumpHeight);
}

void UHDAPlayerMovementComponent::CalculateDashVelocity(FVector& OutVelocity) const
{
	FVector Direction = GetOwner()->GetActorForwardVector();

	if (IsMovingOnGround())
	{
		Direction = GetOwner()->GetActorRightVector();
		Direction = Direction.Cross(CurrentFloor.HitResult.Normal);
	}

	OutVelocity = Direction * DashSpeed;
}
