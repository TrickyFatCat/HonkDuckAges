// 


#include "HDADoorBase.h"

#include "TrickyUtilityLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TimelineComponent.h"
#include "Door/DoorStateControllerComponent.h"
#include "Lock/LockStateControllerComponent.h"
#include "LockKey/LockKeyType.h"


AHDADoorBase::AHDADoorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	DoorStateControllerComponent = CreateDefaultSubobject<UDoorStateControllerComponent>(TEXT("DoorStateController"));
	LockStateControllerComponent = CreateDefaultSubobject<ULockStateControllerComponent>(TEXT("LockStateController"));
	DoorAnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorAnimationTimeline"));
	DoorAnimationTimeline->SetTimelineLength(1.0f);

#if WITH_EDITOR
	ForwardVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardVector"));
	ForwardVector->SetupAttachment(GetRootComponent());
	ForwardVector->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	ForwardVector->ArrowLength = 100.0f;
	ForwardVector->ArrowSize = 2.f;

	DebugText_F = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>("DebugText_F");
	DebugText_F->SetupAttachment(ForwardVector);
	DebugText_F->SetHiddenInGame(true);
	DebugText_F->SetRelativeLocation(FVector(30.0f, 0.0f, 10.f));
	DebugText_F->SetHorizontalAlignment(EHTA_Center);
	DebugText_F->SetWorldSize(34.f);
	DebugText_F->SetTextRenderColor(FColor::Magenta);

	DebugText_B = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>("DebugText_B");
	DebugText_B->SetupAttachment(ForwardVector);
	DebugText_B->SetHiddenInGame(true);
	DebugText_B->SetRelativeLocation(FVector(-30.0f, 0.0f, 10.f));
	DebugText_B->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	DebugText_B->SetHorizontalAlignment(EHTA_Center);
	DebugText_B->SetWorldSize(34.f);
	DebugText_B->SetTextRenderColor(FColor::Magenta);
#endif
}

#if WITH_EDITOR
void AHDADoorBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName InitialStateName = GET_MEMBER_NAME_CHECKED(AHDADoorBase, InitialState);

	if (PropertyName == InitialStateName)
	{
		DoorStateControllerComponent->SetInitialState(InitialState);
		const ELockState LockState = InitialState == EDoorState::Locked ? ELockState::Locked : ELockState::Unlocked;
		LockStateControllerComponent->SetInitialState(LockState);
	}

	const FName RequiredKeyName = GET_MEMBER_NAME_CHECKED(AHDADoorBase, RequiredKey);

	if (PropertyName == RequiredKeyName)
	{
		LockStateControllerComponent->SetRequiredKey(RequiredKey);
	}

	UpdateDebugText();
}
#endif

void AHDADoorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR
	AHDADoorBase::UpdateDebugText();
#endif

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		LockStateControllerComponent->OnLockStateChanged.AddUniqueDynamic(this, &AHDADoorBase::HandleLockStateChanged);
		DoorStateControllerComponent->OnDoorStateTransitionStarted.AddUniqueDynamic(this,
			&AHDADoorBase::HandleTransitionStarted);
		DoorStateControllerComponent->OnDoorTransitionReversed.AddUniqueDynamic(this,
			&AHDADoorBase::HandleTransitionReversed);

		ensureMsgf(IsValid(OpenAnimationCurve), TEXT("%s open animation curve isn't set"), *GetActorNameOrLabel());

		if (bSeparateAnimationCurve)
		{
			ensureMsgf(IsValid(CloseAnimationCurve),
			           TEXT("%s close animation curve isn't set"),
			           *GetActorNameOrLabel());
		}

		if (IsValid(OpenAnimationCurve))
		{
			FOnTimelineFloat DoorAnimationTimelineDelegate;
			DoorAnimationTimelineDelegate.BindUFunction(this, FName("AnimateDoor"));
			DoorAnimationTimeline->AddInterpFloat(OpenAnimationCurve,
			                                      DoorAnimationTimelineDelegate,
			                                      NAME_None,
			                                      AnimationTrackName);

			FOnTimelineEvent DoorAnimationFinishedDelegate;
			DoorAnimationFinishedDelegate.BindUFunction(this, FName("FinishAnimation"));
			DoorAnimationTimeline->SetTimelineFinishedFunc(DoorAnimationFinishedDelegate);

			if (!ensureMsgf(OpenAnimationDuration > 0.0,
			                TEXT("%s open animation duration can't be less or equal zero. Force it to 1"),
			                *GetActorNameOrLabel()))
			{
				OpenAnimationDuration = 1.0f;
			}

			if (bSeparateAnimationDuration)
			{
				if (!ensureMsgf(CloseAnimationDuration > 0.0,
				                TEXT("%s close animation duration can't be less or equal zero. Force it to 1"),
				                *GetActorNameOrLabel()))
				{
					CloseAnimationDuration = 1.0f;
				}
			}

			UTrickyUtilityLibrary::CalculateTimelinePlayRate(DoorAnimationTimeline, OpenAnimationDuration);
		}
	}
}

bool AHDADoorBase::OpenDoor_Implementation(const bool bTransitImmediately)
{
	return IDoorInterface::Execute_OpenDoor(DoorStateControllerComponent, bTransitImmediately);
}

bool AHDADoorBase::CloseDoor_Implementation(const bool bTransitImmediately)
{
	return IDoorInterface::Execute_CloseDoor(DoorStateControllerComponent, bTransitImmediately);
}

bool AHDADoorBase::UnlockDoor_Implementation(const bool bTransitImmediately)
{
	return IDoorInterface::Execute_UnlockDoor(DoorStateControllerComponent, bTransitImmediately);
}

bool AHDADoorBase::LockDoor_Implementation(const bool bTransitImmediately)
{
	return IDoorInterface::Execute_LockDoor(DoorStateControllerComponent, bTransitImmediately);
}

bool AHDADoorBase::DisableDoor_Implementation(const bool bTransitImmediately)
{
	return IDoorInterface::Execute_DisableDoor(DoorStateControllerComponent, bTransitImmediately);
}

bool AHDADoorBase::ReverseDoorStateTransition_Implementation()
{
	return IDoorInterface::Execute_ReverseDoorStateTransition(DoorStateControllerComponent);
}

void AHDADoorBase::FinishAnimation()
{
	IDoorInterface::Execute_FinishDoorStateTransition(DoorStateControllerComponent);
}

void AHDADoorBase::HandleLockStateChanged(ULockStateControllerComponent* Component,
                                          ELockState NewState,
                                          bool bChangedImmediately)
{
	if (NewState == ELockState::Unlocked)
	{
		Execute_UnlockDoor(DoorStateControllerComponent, true);
	}
}

void AHDADoorBase::HandleTransitionStarted(UDoorStateControllerComponent* Component,
                                           const EDoorState TargetState)
{
	CalculateAnimationPlayRate(TargetState);
	SwapAnimationCurve(TargetState);

	switch (TargetState)
	{
	case EDoorState::Opened:
		DoorAnimationTimeline->PlayFromStart();
		break;

	case EDoorState::Closed:
		DoorAnimationTimeline->ReverseFromEnd();
		break;
	}
}

void AHDADoorBase::HandleTransitionReversed(UDoorStateControllerComponent* Component,
                                            const EDoorState NewTargetState)
{
	CalculateAnimationPlayRate(NewTargetState);
	SwapAnimationCurve(NewTargetState);

	switch (NewTargetState)
	{
	case EDoorState::Opened:
		DoorAnimationTimeline->Play();
		break;

	case EDoorState::Closed:
		DoorAnimationTimeline->Reverse();
		break;
	}
}

void AHDADoorBase::CalculateAnimationPlayRate(const EDoorState State) const
{
	if (!bSeparateAnimationDuration)
	{
		return;
	}

	switch (State)
	{
	case EDoorState::Opened:
		UTrickyUtilityLibrary::CalculateTimelinePlayRate(DoorAnimationTimeline, OpenAnimationDuration);
		break;

	case EDoorState::Closed:
		UTrickyUtilityLibrary::CalculateTimelinePlayRate(DoorAnimationTimeline, CloseAnimationDuration);
		break;
	}
}

void AHDADoorBase::SwapAnimationCurve(const EDoorState State) const
{
	if (!bSeparateAnimationCurve)
	{
		return;
	}

	switch (State)
	{
	case EDoorState::Opened:
		DoorAnimationTimeline->SetFloatCurve(OpenAnimationCurve, AnimationTrackName);
		break;

	case EDoorState::Closed:
		DoorAnimationTimeline->SetFloatCurve(CloseAnimationCurve, AnimationTrackName);
		break;
	}
}


#if WITH_EDITOR
void AHDADoorBase::UpdateDebugText()
{
	FString DebugText = FString::Printf(TEXT("%s\n"), *GetActorNameOrLabel());
	FString KeyName = TEXT("NONE");

	if (IsValid(RequiredKey))
	{
		KeyName = RequiredKey->GetName().RightChop(12);
		KeyName = KeyName.LeftChop(2);
	}

	DebugText = DebugText.Appendf(TEXT("Key: %s\n"), *KeyName);
	const FString StateName = StaticEnum<EDoorState>()->GetNameStringByValue(static_cast<int64>(InitialState));
	DebugText = DebugText.Appendf(TEXT("Initial State: %s\n"), *StateName);

	if (bSeparateAnimationDuration)
	{
		DebugText = DebugText.Appendf(TEXT("Open Animation Duration: %.2f\n"), OpenAnimationDuration);
		DebugText = DebugText.Appendf(TEXT("Close Animation Duration: %.2f\n"), CloseAnimationDuration);
	}
	else
	{
		DebugText = DebugText.Appendf(TEXT("Animation Duration: %.2f\n"), OpenAnimationDuration);
	}

	DebugText_F->SetText(FText::FromString(DebugText));
	DebugText_B->SetText(FText::FromString(DebugText));
}
#endif
