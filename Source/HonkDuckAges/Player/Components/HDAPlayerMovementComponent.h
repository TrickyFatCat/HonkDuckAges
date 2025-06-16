// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HDAPlayerMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HONKDUCKAGES_API UHDAPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UHDAPlayerMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Character Movement: Jumping / Falling",
		meta=(ClampMin=0, UIMin=0, Delta=1, ForceUnits="Centimeters"))
	float JumpHeight = 100.f;

private:
	float CalculateJumpHeight() const;
};
