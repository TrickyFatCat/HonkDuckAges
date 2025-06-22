// 


#include "HDADoorAutoBase.h"

#include "Components/BoxComponent.h"
#include "Door/DoorStateControllerComponent.h"


AHDADoorAutoBase::AHDADoorAutoBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ActivationTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationTrigger"));
	ActivationTriggerComponent->SetupAttachment(GetRootComponent());
	ActivationTriggerComponent->SetCollisionProfileName(TEXT("PlayerTrigger"));
}

void AHDADoorAutoBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();

	if (!IsValid(World) || !World->IsGameWorld())
	{
		return;
	}

	ActivationTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AHDADoorAutoBase::HandleTriggerEntered);
	ActivationTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &AHDADoorAutoBase::HandleTriggerExited);
}

void AHDADoorAutoBase::HandleTriggerEntered(UPrimitiveComponent* OverlappedComponent,
                                            AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex,
                                            bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	const EDoorState CurrentState = DoorStateControllerComponent->GetCurrentState();

	switch (CurrentState)
	{
	case EDoorState::Closed:
		Execute_OpenDoor(this, false);
		break;

	case EDoorState::Locked:
		Execute_UnlockDoor(this, true);
		break;

	case EDoorState::Transition:
		Execute_ReverseDoorStateTransition(this);
		break;
	}
}

void AHDADoorAutoBase::HandleTriggerExited(UPrimitiveComponent* OverlappedComponent,
                                           AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp,
                                           int32 OtherBodyIndex)
{
	const EDoorState CurrentState = DoorStateControllerComponent->GetCurrentState();

	switch (CurrentState)
	{
	case EDoorState::Opened:
		Execute_CloseDoor(this, false);
		break;

	case EDoorState::Transition:
		Execute_ReverseDoorStateTransition(this);
		break;
	}
}
