// 

#pragma once

#include "CoreMinimal.h"
#include "TrickyInteractionInterface.h"
#include "Button/ButtonInterface.h"
#include "GameFramework/Actor.h"
#include "HDAButtonBase.generated.h"

class UTextRenderComponent;
class UArrowComponent;
class UTimelineComponent;
class UButtonStateControllerComponent;
class UBoxComponent;

UENUM()
enum class EButtonType : uint8
{
	Normal,
	ButtonKey,
	Lever
};

UCLASS(Abstract, PrioritizeCategories="Button")
class HONKDUCKAGES_API AHDAButtonBase : public AActor, public ITrickyInteractionInterface, public IButtonInterface
{
	GENERATED_BODY()

public:
	AHDAButtonBase();

protected:
	virtual void PostInitializeComponents() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Button")
	FInteractionData InteractionData{
		FText::FromString(TEXT("Press")),
		true,
		0
	};

	virtual EInteractionResult ForceInteraction_Implementation(AActor* Interactor) override;

	virtual bool PressButton_Implementation(const bool bTransitImmediately) override;

	virtual bool ReleaseButton_Implementation(const bool bTransitImmediately) override;

	virtual bool ReverseButtonStateTransition_Implementation() override;

	virtual bool DisableButton_Implementation(const bool bTransitImmediately) override;

	virtual bool EnableButton_Implementation(const bool bTransitImmediately) override;

	virtual bool ForceButtonState_Implementation(const EButtonState NewState, const bool bTransitImmediately) override;

	UFUNCTION(BlueprintGetter, Category="Button")
	float GetKeyPressDuration() const { return KeyPressDuration; }

	UFUNCTION(BlueprintSetter, Category="Button")
	void SetKeyPressDuration(const float NewDuration) { KeyPressDuration = NewDuration; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> InteractionTrigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UButtonStateControllerComponent> StateControllerComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UTimelineComponent> AnimationComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button")
	EButtonType ButtonType = EButtonType::Normal;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Button", meta=(InvalidEnumValues="EButtonState::Transition"))
	EButtonState InitialState = EButtonState::Released;

	UPROPERTY(EditAnywhere,
		BlueprintGetter=GetKeyPressDuration,
		BlueprintSetter=SetKeyPressDuration,
		Category="Button",
		meta=(ClampMin=0, UIMin=0, Delta=0.5, ForceUnits="Seconds", EditCondition="ButtonType == EButtonType::ButtonKey"
		))
	float KeyPressDuration = 0.5f;

	UPROPERTY(BlueprintReadOnly, Category="Button")
	FTimerHandle KeyPressTimerHandle;

	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category="Button",
		meta=(ClampMin=0.0f, UIMin=0.0f, Delta=0.1f, ForceUnits="Seconds"))
	float PressAnimationDuration = 0.25f;

	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category="Button")
	bool bSeparateAnimationDuration = false;

	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category="Button",
		meta=(ClampMin=0.0f, UIMin=0.0f, Delta=0.1f, ForceUnits="Seconds",
			EditCondition="bSeparateAnimationDuration == true"))
	float ReleaseAnimationDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Button")
	UCurveFloat* AnimationCurve = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category="Button")
	void AnimationProcessed(const float Progress);

	UFUNCTION(BlueprintImplementableEvent, Category="Button")
	void AnimationFinished();

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UTextRenderComponent> InfoText = nullptr;
#endif

private:
	UFUNCTION()
	void HandleTriggerEntered(UPrimitiveComponent* OverlappedComponent,
	                          AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex,
	                          bool bFromSweep,
	                          const FHitResult& SweepResult);

	UFUNCTION()
	void HandleTriggerExited(UPrimitiveComponent* OverlappedComponent,
	                         AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp,
	                         int32 OtherBodyIndex);

	UFUNCTION()
	void HandleStateChanged(UButtonStateControllerComponent* Component,
	                        const EButtonState NewState,
	                        const bool bChangedImmediately);

	UFUNCTION()
	void HandleTransitionStarted(UButtonStateControllerComponent* Component,
	                             const EButtonState TargetState);

	UFUNCTION()
	void HandleTransitionReversed(UButtonStateControllerComponent* Component,
	                              const EButtonState NewTargetState);

	void CalculateAnimationPlayRate(const EButtonState State) const;

	UFUNCTION()
	void ProcessAnimation(const float Progress);

	UFUNCTION()
	void FinishAnimation();

	UFUNCTION()
	void ReleaseKey();

#if WITH_EDITOR
	void UpdateInfoText();
#endif
};
