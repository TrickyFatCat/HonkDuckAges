// 


#include "HDAWeaponStateController.h"


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
	return true;
}
