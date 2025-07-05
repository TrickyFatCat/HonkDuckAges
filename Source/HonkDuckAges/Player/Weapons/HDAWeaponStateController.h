// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDAWeaponStateController.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Shooting,
	OutOfAmmo,
	Disabled,
	Transition
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponStateChangedDynamicSignature,
                                               UHDAWeaponStateController*, Component,
                                               EWeaponState, NewState,
                                               bool, bTransitImmediately);

UCLASS(ClassGroup=(PlayerWeapon))
class HONKDUCKAGES_API UHDAWeaponStateController : public UActorComponent
{
	GENERATED_BODY()

public:
	UHDAWeaponStateController();

protected:
	virtual void InitializeComponent() override;

public:
	UPROPERTY(BlueprintAssignable, Category="WeaponStateController")
	FOnWeaponStateChangedDynamicSignature OnWeaponStateChanged;

	UFUNCTION(BlueprintCallable, Category=("WeaponStateController"))
	bool TransitToIdle(const bool bTransitImmediately);

	UFUNCTION(BlueprintCallable, Category=("WeaponStateController"))
	bool TransitToShooting(const bool bTransitImmediately);

	UFUNCTION(BluepirntCallable, Category=("WeaponStateController"))
	bool TransitToOutOfAmmo(const bool bTransitImmediately);
	
	UFUNCTION(BluepirntCallable, Category=("WeaponStateController"))
	bool TransitToDisabled(const bool bTransitImmediately);
	
	UFUNCTION(BlueprintCallable)
	bool FinishStateTransition();

	UFUNCTION(BlueprintGetter)
	EWeaponState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintGetter)
	EWeaponState GetTargetState() const { return TargetState; }

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetCurrentState, Category="WeaponStateController")
	EWeaponState CurrentState = EWeaponState::Disabled;

	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetTargetState, Category="WeaponStateController")
	EWeaponState TargetState = EWeaponState::Idle;

	UFUNCTION()
	bool ChangeState(const EWeaponState NewState, const bool bTransitImmediately);
};
