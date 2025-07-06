// 


#include "HDAWeaponStateController.h"

DEFINE_LOG_CATEGORY(LogWeaponStateController)

UHDAWeaponStateController::UHDAWeaponStateController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHDAWeaponStateController::InitializeComponent()
{
	Super::InitializeComponent();
}

bool UHDAWeaponStateController::TransitToIdle(const bool bTransitImmediately)
{
	return ChangeState(EWeaponState::Idle, bTransitImmediately);
}

bool UHDAWeaponStateController::TransitToShooting(const bool bTransitImmediately)
{
	return ChangeState(EWeaponState::Shooting, bTransitImmediately);
}

bool UHDAWeaponStateController::TransitToOutOfAmmo(const bool bTransitImmediately)
{
	return ChangeState(EWeaponState::OutOfAmmo, bTransitImmediately);
}

bool UHDAWeaponStateController::TransitToDisabled(const bool bTransitImmediately)
{
	return ChangeState(EWeaponState::Disabled, bTransitImmediately);
}

bool UHDAWeaponStateController::FinishStateTransition()
{
	if (CurrentState != EWeaponState::Transition)
	{
		return false;
	}

	return ChangeState(TargetState, true);
}

bool UHDAWeaponStateController::ChangeState(const EWeaponState NewState, const bool bTransitImmediately)
{
	if (CurrentState == NewState || NewState == EWeaponState::Transition)
	{
		return false;
	}

	TargetState = NewState;
	CurrentState = bTransitImmediately ? TargetState : EWeaponState::Transition;
	OnWeaponStateChanged.Broadcast(this,
	                               CurrentState,
	                               CurrentState == EWeaponState::Transition ? true : bTransitImmediately);
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	auto GetStateName = [](EWeaponState State) -> FString
	{
		return StaticEnum<EWeaponState>()->GetNameStringByValue(static_cast<int64>(State));
	};

	const FString CurrentStateName = GetStateName(CurrentState);

	if (CurrentState == EWeaponState::Transition)
	{
		const FString TargetStateName = GetStateName(TargetState);
		const FString Message = FString::Printf(TEXT("%s state changed to %s. Target State is %s"),
		                                        *GetOwner()->GetActorNameOrLabel(),
		                                        *CurrentStateName,
		                                        *TargetStateName);
	}
	else
	{
		const FString Message = FString::Printf(TEXT("%s state changed to %s"),
		                                        *GetOwner()->GetActorNameOrLabel(),
		                                        *CurrentStateName);
		PrintLog(Message);
	}
#endif
	return true;
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING
void UHDAWeaponStateController::PrintLog(const FString& Message)
{
	UE_LOG(LogWeaponStateController, Log, TEXT("%s"), *Message);
}
#endif
