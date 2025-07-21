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
	DefaultBrakingDecelerationFalling = BrakingDecelerationFalling;

	DashSpeed = DashDistance / DashDuration;
	JumpZVelocity = CalculateJumpSpeed(JumpHeight);
	DashCharges = DashMaxCharges;
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

#if WITH_EDITOR
void UHDAPlayerMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName JumpHeightName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpHeight);
	const FName GravityScaleName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, GravityScale);
	const FName JumpZVelocityName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpZVelocity);

	if (PropertyName == JumpHeightName || PropertyName == GravityScaleName || PropertyName == JumpZVelocityName)
	{
		JumpZVelocity = CalculateJumpSpeed(JumpHeight);
	}
}
#endif

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

	DashCharges += CachedDashCharges;
	DashCharges = FMath::Min(DashCharges, DashMaxCharges);
	CachedDashCharges = 0;
	GravityScale = DefaultGravityScale;

	if (DashCharges < DashMaxCharges)
	{
		StartDashCooldown();
	}
}

void UHDAPlayerMovementComponent::StartDashing(const FVector& Direction)
{
	if (!bCanDash || IsDashing() || Direction.IsNearlyZero() || DashCharges <= 0)
	{
		return;
	}

	DashCharges--;
	DashCharges = FMath::Max(0, DashCharges);

	GravityScale = 0.f;
	BrakingFrictionFactor = 0.f;
	BrakingDecelerationWalking = 0.f;
	AirControl = 0.f;
	BrakingDecelerationFalling = 0.f;

	StopMovementImmediately();
	Launch(DashSpeed * Direction.GetSafeNormal());

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(DashDurationTimer,
	                      this,
	                      &UHDAPlayerMovementComponent::FinishDashing,
	                      DashDuration,
	                      false);
	OnDashStarted.Broadcast();
}

bool UHDAPlayerMovementComponent::IsDashing() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(DashDurationTimer);
}

void UHDAPlayerMovementComponent::FinishDashing()
{
	if (!IsDashing())
	{
		return;
	}

	Velocity *= PostDashVelocityFactor;

	GravityScale = IsFalling() ? FallingGravityScale : DefaultGravityScale;

	BrakingFrictionFactor = DefaultBrakingFrictionFactor;
	BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	AirControl = DefaultAirControl;
	BrakingDecelerationFalling = DefaultBrakingDecelerationFalling;

	OnDashFinished.Broadcast();
	StartDashCooldown();
}

void UHDAPlayerMovementComponent::HandleDashCooldownFinished()
{
	// Need to do this to avoid a bug when the cooldown timer is still valid in the same frame
	GetWorld()->GetTimerManager().ClearTimer(DashCooldownTimer);

	if (IsFalling())
	{
		CachedDashCharges++;
		CachedDashCharges = FMath::Min(CachedDashCharges, DashMaxCharges);
	}
	else
	{
		DashCharges++;
		DashCharges = FMath::Min(DashCharges, DashMaxCharges);
	}

	if (DashCharges + CachedDashCharges < DashMaxCharges)
	{
		StartDashCooldown();
	}

	OnDashCooldownFinished.Broadcast();
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

FVector UHDAPlayerMovementComponent::GetLateralVelocity() const
{
	FVector LateralVelocity = Velocity;
	LateralVelocity.Z = 0.f;
	return LateralVelocity;
}

float UHDAPlayerMovementComponent::GetLateralSpeed() const
{
	const FVector LateralVelocity = GetLateralVelocity();
	return LateralVelocity.Size();
}

float UHDAPlayerMovementComponent::GetNormalizedLateralSpeed() const
{
	if (GetLateralSpeed() <= 0.f || MaxWalkSpeed <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(GetLateralSpeed() / MaxWalkSpeed, 0.f, 1.f);
}

float UHDAPlayerMovementComponent::GetNormalizedVerticalSpeed() const
{
	return Velocity.Z / JumpZVelocity;
}

void UHDAPlayerMovementComponent::ForceLaunch(const float Height,
                                              const FVector& Direction,
                                              const bool bOverrideLateralVelocity)
{
	if (Height <= 0.f || Direction.IsNearlyZero())
	{
		return;
	}

	const float LaunchSpeed = CalculateJumpSpeed(Height);
	FVector FinalVelocity = LaunchSpeed * Direction.GetSafeNormal();

	if (!bOverrideLateralVelocity)
	{
		FinalVelocity += GetLateralVelocity();
	}

	Launch(FinalVelocity);
}

float UHDAPlayerMovementComponent::CalculateJumpSpeed(const float Height) const
{
	return FMath::Sqrt(-2 * GetGravityZ() * Height);
}

void UHDAPlayerMovementComponent::StartDashCooldown()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(DashCooldownTimer))
	{
		return;
	}

	TimerManager.SetTimer(DashCooldownTimer,
	                      this,
	                      &UHDAPlayerMovementComponent::HandleDashCooldownFinished,
	                      DashCooldownDuration,
	                      false);

	OnDashCooldownStarted.Broadcast(DashCooldownTimer);
}
