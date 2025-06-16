// 


#include "HDAPlayerMovementComponent.h"


// Sets default values for this component's properties
UHDAPlayerMovementComponent::UHDAPlayerMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UHDAPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UHDAPlayerMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHDAPlayerMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName JumpHeightName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpHeight);
	const FName GravityScaleName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, GravityScale);

	if (PropertyName == JumpHeightName || PropertyName == GravityScaleName)
	{
		JumpZVelocity = CalculateJumpZVelocity();
	}
	
	const FName JumpZVelocityName = GET_MEMBER_NAME_CHECKED(UHDAPlayerMovementComponent, JumpZVelocity);
	
	if (PropertyName == JumpZVelocityName)
	{
		JumpHeight = GetMaxJumpHeight();
	}
}

float UHDAPlayerMovementComponent::CalculateJumpZVelocity() const
{
	return FMath::Sqrt(-2 * GetGravityZ() * JumpHeight);
}

