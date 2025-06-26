// 

#pragma once

#include "CoreMinimal.h"
#include "Door/DoorInterface.h"
#include "GameFramework/Actor.h"
#include "HDADoorBase.generated.h"

class UTimelineComponent;
class UTextRenderComponent;
class UArrowComponent;
class ULockKeyType;
class ULockStateControllerComponent;
class UDoorStateControllerComponent;

UCLASS(PrioritizeCategories="Door")
class HONKDUCKAGES_API AHDADoorBase : public AActor, public IDoorInterface
{
	GENERATED_BODY()

public:
	AHDADoorBase();

protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostInitializeComponents() override;

public:
	virtual bool OpenDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool CloseDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool UnlockDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool LockDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool DisableDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool ReverseDoorStateTransition_Implementation() override;

protected:
	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category="Door",
		meta=(InvalidEnumValues="EDoorState::Transition"))
	EDoorState InitialState = EDoorState::Closed;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Door")
	TSubclassOf<ULockKeyType> RequiredKey = nullptr;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Door")
	UCurveFloat* DoorAnimationCurve = nullptr;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Door",
		meta=(ClampMin=0.0f, UIMin=0.0f, Delta=0.1f, ForceUnits="Seconds"))
	float AnimationDuration = 0.25f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UDoorStateControllerComponent> DoorStateControllerComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<ULockStateControllerComponent> LockStateControllerComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Components")
	TObjectPtr<UTimelineComponent> DoorAnimationTimeline = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void AnimateDoor(const float Progress);

private:
	UFUNCTION()
	void FinishAnimation();

	UFUNCTION()
	void HandleLockStateChanged(ULockStateControllerComponent* Component,
	                            ELockState NewState,
	                            bool bChangedImmediately);

	UFUNCTION()
	void HandleTransitionStarted(UDoorStateControllerComponent* Component,
	                             const EDoorState TargetState);

	UFUNCTION()
	void HandleTransitionReversed(UDoorStateControllerComponent* Component,
	                              const EDoorState NewTargetState);

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	TObjectPtr<UTextRenderComponent> DebugText_F = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	TObjectPtr<UTextRenderComponent> DebugText_B = nullptr;

#if WITH_EDITOR
	virtual void UpdateDebugText();
#endif
#endif
};
