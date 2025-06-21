// 


#include "HDADoorBase.h"

#include "Components/ArrowComponent.h"
#include "Door/DoorStateControllerComponent.h"
#include "Lock/LockStateControllerComponent.h"


AHDADoorBase::AHDADoorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	DoorStateControllerComponent->CreateDefaultSubobject<UDoorStateControllerComponent>(TEXT("DoorStateController"));
	LockStateControllerComponent->CreateDefaultSubobject<ULockStateControllerComponent>(TEXT("LockStateController"));

#if WITH_EDITOR
	ForwardVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardVector"));
	ForwardVector->SetupAttachment(GetRootComponent());
	ForwardVector->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	ForwardVector->ArrowLength = 100.0f;
	ForwardVector->ArrowSize = 2.f;
#endif
}

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
}

void AHDADoorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		LockStateControllerComponent->OnLockStateChanged.AddDynamic(this, &AHDADoorBase::HandleLockStateChanged);
	}
}

bool AHDADoorBase::OpenDoor_Implementation(const bool bTransitImmediately)
{
	return DoorStateControllerComponent->OpenDoor(bTransitImmediately);
}

bool AHDADoorBase::CloseDoor_Implementation(const bool bTransitImmediately)
{
	return DoorStateControllerComponent->CloseDoor(bTransitImmediately);
}

bool AHDADoorBase::UnlockDoor_Implementation(const bool bTransitImmediately)
{
	return DoorStateControllerComponent->UnlockDoor(bTransitImmediately);
}

bool AHDADoorBase::LockDoor_Implementation(const bool bTransitImmediately)
{
	return DoorStateControllerComponent->LockDoor(bTransitImmediately);
}

bool AHDADoorBase::DisableDoor_Implementation(const bool bTransitImmediately)
{
	return DoorStateControllerComponent->DisableDoor(bTransitImmediately);
}

bool AHDADoorBase::ReverseDoorStateTransition_Implementation()
{
	return DoorStateControllerComponent->ReverseDoorStateTransition();
}

void AHDADoorBase::HandleLockStateChanged(ULockStateControllerComponent* Component,
                                          ELockState NewState,
                                          bool bChangedImmediately)
{
	if (NewState == ELockState::Unlocked)
	{
		DoorStateControllerComponent->UnlockDoor(true);
	}
}
