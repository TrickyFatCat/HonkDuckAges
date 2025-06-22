// 


#include "HDADoorAutoBase.h"

#include "Components/BoxComponent.h"
#include "Door/DoorStateControllerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Lock/LockInterface.h"
#include "Lock/LockStateControllerComponent.h"


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

	ActivationTriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AHDADoorAutoBase::HandleTriggerEntered);
	ActivationTriggerComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AHDADoorAutoBase::HandleTriggerExited);
	DoorStateControllerComponent->OnDoorStateChanged.AddUniqueDynamic(this, &AHDADoorAutoBase::HandleDoorStateChanged);
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
		if (ILockInterface::Execute_Unlock(LockStateControllerComponent, OtherActor, true))
		{
			Execute_OpenDoor(this, false);
		}
		break;

	case EDoorState::Transition:
		Execute_ReverseDoorStateTransition(this);
		break;
	}

	if (bIsOneWay)
	{
		const FVector Location = OtherActor->GetActorLocation();
		EnterDirection = UKismetMathLibrary::GetDirectionUnitVector(Location, GetActorLocation());
	}
}

void AHDADoorAutoBase::HandleTriggerExited(UPrimitiveComponent* OverlappedComponent,
                                           AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp,
                                           int32 OtherBodyIndex)
{
	const EDoorState CurrentState = DoorStateControllerComponent->GetCurrentState();

	if (bIsOneWay)
	{
		const FVector Location = OtherActor->GetActorLocation();
		ExitDirection = UKismetMathLibrary::GetDirectionUnitVector(Location, GetActorLocation());
		const int32 DotProductEnter = FMath::CeilToInt(FVector::DotProduct(EnterDirection, GetActorForwardVector()));
		const int32 DotProductExit = FMath::CeilToInt(FVector::DotProduct(ExitDirection, GetActorForwardVector()));
		bWantsToBeDisabled = DotProductEnter > DotProductExit;
	}

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

void AHDADoorAutoBase::HandleDoorStateChanged(UDoorStateControllerComponent* Component,
                                              const EDoorState NewState,
                                              const bool bChangedImmediately)
{
	if (NewState == EDoorState::Closed && bWantsToBeDisabled)
	{
		Execute_DisableDoor(this, false);
		ActivationTriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
