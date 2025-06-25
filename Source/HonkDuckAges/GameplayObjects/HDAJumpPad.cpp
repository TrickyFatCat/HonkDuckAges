// 


#include "HDAJumpPad.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "HonkDuckAges/Player/Components/HDAPlayerMovementComponent.h"


AHDAJumpPad::AHDAJumpPad()
{
	PrimaryActorTick.bCanEverTick = true;

	ActivationTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationTrigger"));
	SetRootComponent(ActivationTriggerComponent);
	ActivationTriggerComponent->SetCollisionProfileName(TEXT("PlayerTrigger"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

#if WITH_EDITOR
	UpVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("UpVector"));
	UpVector->SetupAttachment(GetRootComponent());
	UpVector->SetRelativeRotation(FRotator(90, 0, 0));
	UpVector->SetArrowColor(FColor::Purple);
	UpVector->SetArrowSize(1.f);

	TextDebug = CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(TEXT("TextDebug"));
	TextDebug->SetupAttachment(UpVector);
	TextDebug->SetRelativeLocation(FVector(0, 0, -50));
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

	MovementComponent->ForceLaunch(JumpHeight, GetActorUpVector());
}

#if WITH_EDITOR
void AHDAJumpPad::UpdateDebugInfo() const
{
	UpVector->SetArrowLength(JumpHeight);
	TextDebug->SetText(FText::FromString(FString::Printf(TEXT("Jump Height: %.2f m"), JumpHeight / 100)));
}
#endif
