// 


#include "HDAJumpPad.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Door/DoorInterface.h"
#include "GameplayObject/GameplayObjectStateControllerComponent.h"
#include "HonkDuckAges/Player/Components/HDAPlayerMovementComponent.h"


AHDAJumpPad::AHDAJumpPad()
{
	PrimaryActorTick.bCanEverTick = true;

	ActivationTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationTrigger"));
	SetRootComponent(ActivationTriggerComponent);
	ActivationTriggerComponent->SetCollisionProfileName(TEXT("PlayerTrigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	StateControllerComponent = CreateDefaultSubobject<UGameplayObjectStateControllerComponent>(
		TEXT("StateControllerComponent"));

#if WITH_EDITOR
	UpVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("UpVector"));
	UpVector->SetupAttachment(GetRootComponent());
	UpVector->SetRelativeRotation(FRotator(90, 0, 0));
	UpVector->SetArrowColor(FColor::Purple);
	UpVector->SetArrowSize(1.f);

	TextDebug = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("TextDebug"));
	TextDebug->SetupAttachment(UpVector);
	TextDebug->SetHiddenInGame(true);
	TextDebug->SetWorldSize(22);
	TextDebug->SetRelativeLocation(FVector(20, 0, -70));
	TextDebug->SetHorizontalAlignment(EHTA_Center);
	TextDebug->SetTextRenderColor(FColor::Purple);
#endif
}

void AHDAJumpPad::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if WITH_EDITOR
	UpdateDebugInfo();
#endif

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		ActivationTriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AHDAJumpPad::HandleTriggerEntered);
		StateControllerComponent->OnGameplayObjectStateChanged.AddUniqueDynamic(this, &AHDAJumpPad::HandleStateChanged);
	}
}

void AHDAJumpPad::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName JumpHeightName = GET_MEMBER_NAME_CHECKED(AHDAJumpPad, JumpHeight);

	if (PropertyName == JumpHeightName)
	{
		UpdateDebugInfo();
	}

	const FName InitialStateName = GET_MEMBER_NAME_CHECKED(AHDAJumpPad, InitialState);

	if (PropertyName == InitialStateName)
	{
		if (InitialState == EGameplayObjectState::Transition)
		{
			InitialState = EGameplayObjectState::Active;
		}

		StateControllerComponent->SetInitialState(InitialState);
		HandleStateChanged(nullptr, InitialState, true);
	}
}

void AHDAJumpPad::HandleTriggerEntered(UPrimitiveComponent* OverlappedComponent,
                                       AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp,
                                       int32 OtherBodyIndex,
                                       bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	UHDAPlayerMovementComponent* MovementComponent = OtherActor->FindComponentByClass<UHDAPlayerMovementComponent>();

	if (!IsValid(MovementComponent))
	{
		return;
	}

	MovementComponent->ForceLaunch(JumpHeight, GetActorUpVector(), true);
}

void AHDAJumpPad::HandleStateChanged(UGameplayObjectStateControllerComponent* Component,
                                     const EGameplayObjectState NewState,
                                     const bool bChangedImmediately)
{
	switch (NewState)
	{
	case EGameplayObjectState::Active:
		ActivationTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;

	case EGameplayObjectState::Disabled:
		ActivationTriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

#if WITH_EDITOR
void AHDAJumpPad::UpdateDebugInfo() const
{
	UpVector->SetArrowLength(JumpHeight);
	FString DebugText = FString::Printf(TEXT("%s\n"), *GetActorNameOrLabel());
	DebugText = DebugText.Appendf(TEXT("Jump Height: %.2f m\n"), JumpHeight / 100);
	const FString StateName = StaticEnum<EGameplayObjectState>()->
		GetNameStringByValue(static_cast<int64>(InitialState));
	DebugText = DebugText.Appendf(TEXT("Initial State: %s"), *StateName);
	TextDebug->SetText(FText::FromString(DebugText));
}
#endif
