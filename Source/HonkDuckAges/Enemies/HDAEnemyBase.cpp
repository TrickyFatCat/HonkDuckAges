// 


#include "HDAEnemyBase.h"

#include "HonkDuckAges/Shared/Components/HDALifeStateComponent.h"


AHDAEnemyBase::AHDAEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	LifeStateComponent = CreateDefaultSubobject<UHDALifeStateComponent>(TEXT("LifeState"));
}

void AHDAEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		
	}
}

void AHDAEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDAEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHDAEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

