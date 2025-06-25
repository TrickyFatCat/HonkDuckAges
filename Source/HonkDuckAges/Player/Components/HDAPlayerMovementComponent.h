// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HDAPlayerMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashStartedDynamicSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashFinishedDynamicSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDashCooldownStartedDynamicSignature, const FTimerHandle&, CooldownTimer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashCooldownFinishedDynamicSignature);

UCLASS(ClassGroup=(Custom))
class HONKDUCKAGES_API UHDAPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UHDAPlayerMovementComponent();

protected:
	virtual void InitializeComponent() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual bool CanEverJump() const override;

	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;

public:
	UPROPERTY(BlueprintAssignable, Category="Character Movement: Dash")
	FOnDashStartedDynamicSignature OnDashStarted;

	UPROPERTY(BlueprintAssignable, Category="Character Movement: Dash")
	FOnDashFinishedDynamicSignature OnDashFinished;

	UPROPERTY(BlueprintAssignable, Category="Character Movement: Dash")
	FOnDashCooldownStartedDynamicSignature OnDashCooldownStarted;

	UPROPERTY(BlueprintAssignable, Category="Character Movement: Dash")
	FOnDashCooldownFinishedDynamicSignature OnDashCooldownFinished;

	UFUNCTION(BlueprintCallable, Category="Character Movement: Dash")
	void StartDashing(const FVector& Direction);

	UFUNCTION(BlueprintGetter)
	bool IsDashing() const;

	UFUNCTION(BlueprintPure)
	float GetDashCooldownElapsedTime() const;

	UFUNCTION(BlueprintPure)
	float GetDashCooldownRemainingTime() const;

	UFUNCTION(BlueprintPure)
	bool IsDashOnCooldown() const;

	UFUNCTION(BlueprintGetter)
	bool GetCanDash() const { return bCanDash; }

	UFUNCTION(BlueprintSetter)
	void SetCanDash(const bool Value);

	UFUNCTION(BlueprintCallable)
	int32 GetDashCharges() const { return DashCharges; }

	UFUNCTION(BlueprintCallable)
	int32 GetDashMaxCharges() const { return DashMaxCharges; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetCachedDashCharges() const { return CachedDashCharges; }

	UFUNCTION(BlueprintPure)
	FVector GetLateralVelocity() const;

	UFUNCTION(BlueprintPure)
	float GetLateralSpeed() const;

	UFUNCTION(BlueprintCallable)
	void ForceLaunch(const float Height, const FVector& Direction);

protected:
	/**
	 * Defines the gravity scale applied to the character during the falling state.
	 */
	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement (General Settings)",
		meta=(ClampMin=0, UIMin=0))
	float FallingGravityScale = 4.0f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Jumping / Falling",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Centimeters"))
	float JumpHeight = 100.f;

	/**
	 * Defines whether the character is allowed to dash.
	 */
	UPROPERTY(EditDefaultsOnly,
			BlueprintGetter=GetCanDash,
			BlueprintSetter=SetCanDash,
			Category="Character Movement: Dash")
	bool bCanDash = true;

	/**
	 * Defines the maximum number of dashes the character can perform
	 */
	UPROPERTY(EditDefaultsOnly,
		BlueprintGetter=GetDashMaxCharges,
		Category="Character Movement: Dash",
		meta=(ClampMin=1, UIMin=1, Delta=1))
	int32 DashMaxCharges = 2;

	/**
	 * The current number of available dash charges the character can use
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetDashCharges, Category="Character Movement: Dash")
	int32 DashCharges = 2;

	/**
	 * Used while the player is falling to limit dash capabilities during falling. Added to DashCharges on landing.
	 */
	UPROPERTY(VisibleInstanceOnly,
		BlueprintGetter=GetCachedDashCharges,
		Category="Character Movement: Dash")
	int32 CachedDashCharges = 0;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Dash",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Centimeters"))
	float DashDistance = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Movement: Dash")
	float DashSpeed = 200.f;

	/**
	 * Multiplier applied to the character's velocity after completing a dash. 
	 */
	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Dash",
		meta=(ClampMin=0, UIMin=0, ClampMax=1, UIMax=1))
	float PostDashVelocityFactor = 0.25;

	/**
	 * Specifies the duration, in seconds, for which the character performs a dash.
	 */
	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Dash",
		meta=(ClampMin=0, UIMin=0, Delta=0.1, ForceUnits="Seconds"))
	float DashDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Dash",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Seconds"))
	float DashCooldownDuration = 0.4f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character Movement: Dash")
	FTimerHandle DashDurationTimer;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character Movement: Dash")
	FTimerHandle DashCooldownTimer;

private:
	UPROPERTY()
	float DefaultGravityScale = 1.f;

	UPROPERTY()
	float DefaultBrakingFrictionFactor = 1.f;

	UPROPERTY()
	float DefaultBrakingDecelerationWalking = 1.f;

	UPROPERTY()
	float DefaultAirControl = 0.25f;

	UPROPERTY()
	float DefaultBrakingDecelerationFalling = 1.f;

	UFUNCTION()
	void FinishDashing();

	UFUNCTION()
	void HandleDashCooldownFinished();

	float CalculateJumpSpeed(const float Height) const;

	void StartDashCooldown();
};
