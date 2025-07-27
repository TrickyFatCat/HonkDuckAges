// 


#include "HDAButtonBase.h"

#include "TrickyInteractionLibrary.h"
#include "TrickyUtilityLibrary.h"
#include "Button/ButtonStateControllerComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TimelineComponent.h"


AHDAButtonBase::AHDAButtonBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionTrigger"));
	InteractionTrigger->SetupAttachment(GetRootComponent());
	InteractionTrigger->SetCollisionProfileName(TEXT("PlayerTrigger"));
	InteractionTrigger->SetMobility(EComponentMobility::Type::Static);

	StateControllerComponent = CreateDefaultSubobject<UButtonStateControllerComponent>(TEXT("StateController"));
	AnimationComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("AnimationComponent"));
	AnimationComponent->SetTimelineLength(1.f);

#if WITH_EDITOR
	ForwardVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardVector"));
	ForwardVector->SetupAttachment(GetRootComponent());
	ForwardVector->SetMobility(EComponentMobility::Type::Static);
	ForwardVector->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	ForwardVector->ArrowLength = 100.0f;
	ForwardVector->ArrowSize = 2.f;

	InfoText = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("InfoText"));
	InfoText->SetupAttachment(ForwardVector);
	InfoText->SetMobility(EComponentMobility::Type::Static);
	InfoText->SetHiddenInGame(true);
	InfoText->SetRelativeLocation(FVector(50.0f, 0.0f, 150.f));
	InfoText->SetHorizontalAlignment(EHTA_Center);
	InfoText->SetWorldSize(34.f);
	InfoText->SetTextRenderColor(FColor::Magenta);
#endif
}

void AHDAButtonBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR
	UpdateInfoText();
#endif

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		ensureMsgf(AnimationCurve != nullptr,
		           TEXT("AnimationCurve is not set for %s"),
		           *GetActorNameOrLabel());

		InteractionTrigger->OnComponentBeginOverlap.AddUniqueDynamic(this,
		                                                             &AHDAButtonBase::HandleTriggerEntered);
		InteractionTrigger->OnComponentEndOverlap.AddUniqueDynamic(this,
		                                                           &AHDAButtonBase::HandleTriggerExited);

		StateControllerComponent->OnButtonStateChanged.AddUniqueDynamic(this,
		                                                                &AHDAButtonBase::HandleStateChanged);
		StateControllerComponent->OnButtonStateTransitionStarted.AddUniqueDynamic(this,
			&AHDAButtonBase::HandleTransitionStarted);
		StateControllerComponent->OnButtonStateTransitionReversed.AddUniqueDynamic(this,
			&AHDAButtonBase::HandleTransitionReversed);

		if (IsValid(AnimationCurve))
		{
			FOnTimelineFloat ButtonAnimationTimelineDelegate;
			ButtonAnimationTimelineDelegate.BindUFunction(this, FName("ProcessAnimation"));
			AnimationComponent->AddInterpFloat(AnimationCurve,
			                                   ButtonAnimationTimelineDelegate,
			                                   NAME_None,
			                                   TEXT("Progress"));

			FOnTimelineEvent ButtonAnimationFinishedDelegate;
			ButtonAnimationFinishedDelegate.BindUFunction(this, FName("FinishAnimation"));
			AnimationComponent->SetTimelineFinishedFunc(ButtonAnimationFinishedDelegate);

			if (!ensureMsgf(PressAnimationDuration > 0.0,
			                TEXT("%s animation duration can't be less or equal zero. Force it to 1"),
			                *GetActorNameOrLabel()))
			{
				PressAnimationDuration = 1.0f;
			}

			UTrickyUtilityLibrary::CalculateTimelinePlayRate(AnimationComponent, PressAnimationDuration);
		}
	}
}

#if WITH_EDITOR
void AHDAButtonBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName InitialStateName = GET_MEMBER_NAME_CHECKED(AHDAButtonBase, InitialState);

	if (PropertyName == InitialStateName)
	{
		StateControllerComponent->SetInitialState(InitialState);

		switch (InitialState)
		{
		case EButtonState::Released:
			InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			ProcessAnimation(0.f);
			break;

		case EButtonState::Pressed:
			InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			ProcessAnimation(1.f);

		case EButtonState::Disabled:
			InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			break;
		}
	}

	UpdateInfoText();
}
#endif

EInteractionResult AHDAButtonBase::ForceInteraction_Implementation(AActor* Interactor)
{
	bool bResult = false;

	switch (StateControllerComponent->GetCurrentState())
	{
	case EButtonState::Released:
		bResult = IButtonInterface::Execute_PressButton(StateControllerComponent, false);;
		break;

	case EButtonState::Pressed:
		if (ButtonType == EButtonType::Lever)
		{
			bResult = IButtonInterface::Execute_ReleaseButton(StateControllerComponent, false);
		}
		break;
	}

	return bResult ? EInteractionResult::Success : EInteractionResult::Failure;
}

bool AHDAButtonBase::PressButton_Implementation(const bool bTransitImmediately)
{
	return IButtonInterface::Execute_PressButton(StateControllerComponent, bTransitImmediately);
}

bool AHDAButtonBase::ReleaseButton_Implementation(const bool bTransitImmediately)
{
	return IButtonInterface::Execute_ReleaseButton(StateControllerComponent, bTransitImmediately);
}

bool AHDAButtonBase::ReverseButtonStateTransition_Implementation()
{
	return IButtonInterface::Execute_ReverseButtonStateTransition(StateControllerComponent);
}

bool AHDAButtonBase::DisableButton_Implementation(const bool bTransitImmediately)
{
	return IButtonInterface::Execute_DisableButton(StateControllerComponent, bTransitImmediately);
}

bool AHDAButtonBase::EnableButton_Implementation(const bool bTransitImmediately)
{
	return IButtonInterface::Execute_EnableButton(StateControllerComponent, bTransitImmediately);
}

bool AHDAButtonBase::ForceButtonState_Implementation(const EButtonState NewState, const bool bTransitImmediately)
{
	return IButtonInterface::Execute_ForceButtonState(StateControllerComponent, NewState, bTransitImmediately);
}

void AHDAButtonBase::HandleTriggerEntered(UPrimitiveComponent* OverlappedComponent,
                                          AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp,
                                          int32 OtherBodyIndex,
                                          bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	UTrickyInteractionLibrary::AddToInteractionQueue(OtherActor, this);
}

void AHDAButtonBase::HandleTriggerExited(UPrimitiveComponent* OverlappedComponent,
                                         AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp,
                                         int32 OtherBodyIndex)
{
	UTrickyInteractionLibrary::RemoveFromInteractionQueue(OtherActor, this);
}

void AHDAButtonBase::HandleStateChanged(UButtonStateControllerComponent* Component,
                                        const EButtonState NewState,
                                        const bool bChangedImmediately)
{
	switch (NewState)
	{
	case EButtonState::Pressed:
		if (ButtonType == EButtonType::Lever)
		{
			InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		break;
	case EButtonState::Released:
		if (ButtonType == EButtonType::Lever || ButtonType == EButtonType::ButtonKey)
		{
			InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		break;
	}
}

void AHDAButtonBase::HandleTransitionStarted(UButtonStateControllerComponent* Component,
                                             const EButtonState TargetState)
{
	InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CalculateAnimationPlayRate(TargetState);

	switch (TargetState)
	{
	case EButtonState::Pressed:
		AnimationComponent->PlayFromStart();
		break;

	case EButtonState::Released:
		AnimationComponent->ReverseFromEnd();
		break;
	}
}

void AHDAButtonBase::HandleTransitionReversed(UButtonStateControllerComponent* Component,
                                              const EButtonState NewTargetState)
{
	CalculateAnimationPlayRate(NewTargetState);

	switch (NewTargetState)
	{
	case EButtonState::Pressed:
		AnimationComponent->Play();
		break;
	case EButtonState::Released:
		AnimationComponent->Reverse();
		break;
	}
}

void AHDAButtonBase::CalculateAnimationPlayRate(const EButtonState State) const
{
	if (!bSeparateAnimationDuration)
	{
		UTrickyUtilityLibrary::CalculateTimelinePlayRate(AnimationComponent, PressAnimationDuration);
		return;
	}

	switch (State)
	{
	case EButtonState::Pressed:
		UTrickyUtilityLibrary::CalculateTimelinePlayRate(AnimationComponent, PressAnimationDuration);
		break;

	case EButtonState::Released:
		UTrickyUtilityLibrary::CalculateTimelinePlayRate(AnimationComponent, ReleaseAnimationDuration);
		break;
	}
}


void AHDAButtonBase::ProcessAnimation(const float Progress)
{
	AnimationProcessed(Progress);
}


void AHDAButtonBase::FinishAnimation()
{
	IButtonInterface::Execute_FinishButtonStateTransition(StateControllerComponent);
	AnimationFinished();

	switch (ButtonType)
	{
	case EButtonType::ButtonKey:
		if (KeyPressDuration > 0.f)
		{
			GetWorldTimerManager().SetTimer(KeyPressTimerHandle, this, &AHDAButtonBase::ReleaseKey, KeyPressDuration);
		}
		else
		{
			ReleaseKey();
		}
		break;

	case EButtonType::Normal:
		DisableButton_Implementation(true);
		break;
	}
}

void AHDAButtonBase::ReleaseKey()
{
	if (ButtonType != EButtonType::ButtonKey)
	{
		return;
	}

	Execute_ReleaseButton(this, false);
}

#if WITH_EDITOR
void AHDAButtonBase::UpdateInfoText()
{
	FString DebugText = FString::Printf(TEXT("%s\n"), *GetActorNameOrLabel());
	const FString ButtonTypeName = StaticEnum<EButtonType>()->GetNameStringByValue(static_cast<int64>(ButtonType));
	DebugText = DebugText.Appendf(TEXT("Button Type: %s\n"), *ButtonTypeName);
	const FString StateName = StaticEnum<EButtonState>()->GetNameStringByValue(static_cast<int64>(InitialState));
	DebugText = DebugText.Appendf(TEXT("Initial State: %s\n"), *StateName);

	if (!bSeparateAnimationDuration)
	{
		DebugText = DebugText.Appendf(TEXT("Animation Duration: %.2f\n"), PressAnimationDuration);
	}
	else
	{
		DebugText = DebugText.Appendf(TEXT("Press Animation Duration: %.2f\n"), PressAnimationDuration);
		DebugText = DebugText.Appendf(TEXT("Release Animation Duration: %.2f\n"), ReleaseAnimationDuration);
	}

	if (ButtonType == EButtonType::ButtonKey)
	{
		DebugText = DebugText.Appendf(TEXT("Press Duration: %.2f\n"), KeyPressDuration);
	}

	InfoText->SetText(FText::FromString(DebugText));
}
#endif
