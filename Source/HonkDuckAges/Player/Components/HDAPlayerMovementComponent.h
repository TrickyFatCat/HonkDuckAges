// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HDAPlayerMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom))
class HONKDUCKAGES_API UHDAPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UHDAPlayerMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual bool CanEverJump() const override;

	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;

public:
	UFUNCTION(BlueprintCallable, Category="Character Movement: Dash")
	void StartDashing();

	UFUNCTION(BlueprintGetter)
	bool IsDashing() const { return bIsDashing; }

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

protected:
	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Jumping / Falling",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Centimeters"))
	float JumpHeight = 100.f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Dash",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Centimeters"))
	float DashDistance = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Character Movement: Dash")
	float DashSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Dash",
		meta=(ClampMin=0.1, UIMin=0.1, Delta=0.1, ForceUnits="Seconds"))
	float DashDuration = 0.35f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Dash",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Seconds"))
	float DashCooldownDuration = 2.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character Movement: Dash")
	bool bIsDashing = false;

	UPROPERTY(VisibleInstanceOnly,
		BlueprintGetter=GetCanDash,
		BlueprintSetter=SetCanDash,
		Category="Character Movement: Dash")
	bool bCanDash = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character Movement: Dash")
	FTimerHandle DashDurationTimer;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character Movement: Dash")
	FTimerHandle DashCooldownTimer;

private:
	UFUNCTION()
	void FinishDashing();

	UFUNCTION()
	void HandleDashCooldownFinished();

	float CalculateJumpZVelocity() const;

	void CalculateDashVelocity(FVector& OutVelocity) const;
};
