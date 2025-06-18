// 


#include "HDAPlayerMovementComponent.h"


UHDAPlayerMovementComponent::UHDAPlayerMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bMaintainHorizontalGroundVelocity = false;
}

void UHDAPlayerMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	DefaultGravityScale = GravityScale;
	DefaultBrakingFrictionFactor = BrakingFrictionFactor;
	DefaultBrakingDecelerationWalking = BrakingDecelerationWalking;
	DefaultAirControl = AirControl;
	
	DashSpeed = DashDistance / DashDuration;
	JumpZVelocity = CalculateJumpZVelocity();
}

void UHDAPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsFalling() && Velocity.Z < 0.f && !IsDashing())
	{
		GravityScale = FallingGravityScale;
	}
}

void UHDAPlayerMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName JumpHeightName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpHeight);
	const FName GravityScaleName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, GravityScale);
	const FName JumpZVelocityName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpZVelocity);

	if (PropertyName == JumpHeightName || PropertyName == GravityScaleName || PropertyName == JumpZVelocityName)
	{
		JumpZVelocity = CalculateJumpZVelocity();
	}
}

bool UHDAPlayerMovementComponent::CanEverJump() const
{
	return Super::CanEverJump() && !IsDashing();
}

bool UHDAPlayerMovementComponent::DoJump(bool bReplayingMoves, float DeltaTime)
{
	if (IsDashing())
	{
		return false;
	}

	if (!CanEverJump())
	{
		return false;
	}

	GravityScale = DefaultGravityScale;
	return Super::DoJump(bReplayingMoves, DeltaTime);
}

void UHDAPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);

	bCanDash = !IsDashing() && !IsDashOnCooldown();
	GravityScale = DefaultGravityScale;
}

void UHDAPlayerMovementComponent::StartDashing(const FVector& Direction)
{
	if (bIsDashing || IsDashOnCooldown() || !bCanDash)
	{
		return;
	}

	if (Direction.IsNearlyZero())
	{
		return;
	}
	
	bIsDashing = true;
	bCanDash = false;

	GravityScale = 0.f;
	BrakingFrictionFactor = 0.f;
	BrakingDecelerationWalking = 0.f;
	AirControl = 0.f;

	Velocity = DashSpeed * Direction;

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
	
	OnDashStarted.Broadcast();
	OnDashCooldownStarted.Broadcast(DashCooldownTimer);
}

void UHDAPlayerMovementComponent::FinishDashing()
{
	if (!bIsDashing)
	{
		return;
	}

	bIsDashing = false;
	bCanDash = !IsDashOnCooldown() && !IsFalling();

	Velocity *= PostDashVelocityFactor;
	GravityScale = IsFalling() ? FallingGravityScale : DefaultGravityScale;
	BrakingFrictionFactor = DefaultBrakingFrictionFactor;
	BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	AirControl = DefaultAirControl;
	OnDashFinished.Broadcast();
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

void UHDAPlayerMovementComponent::SetGravityScaleToDefault()
{
	GravityScale = DefaultGravityScale;
}

float UHDAPlayerMovementComponent::CalculateJumpZVelocity() const
{
	return FMath::Sqrt(-2 * GetGravityZ() * JumpHeight);
}
