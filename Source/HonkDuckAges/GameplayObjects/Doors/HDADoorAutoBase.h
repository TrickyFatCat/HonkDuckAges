// 

#pragma once

#include "CoreMinimal.h"
#include "HDADoorBase.h"
#include "HDADoorAutoBase.generated.h"

class UBoxComponent;

UCLASS()
class HONKDUCKAGES_API AHDADoorAutoBase : public AHDADoorBase
{
	GENERATED_BODY()

public:
	AHDADoorAutoBase();

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door")
	bool bIsOneWay = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> ActivationTriggerComponent = nullptr;

private:
	FVector EnterDirection = FVector::ZeroVector;

	FVector ExitDirection = FVector::ZeroVector;

	bool bWantsToBeDisabled = false;

	UFUNCTION()
	void HandleTriggerEntered(UPrimitiveComponent* OverlappedComponent,
	                          AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex,
	                          bool bFromSweep,
	                          const FHitResult& SweepResult);

	UFUNCTION()
	void HandleTriggerExited(UPrimitiveComponent* OverlappedComponent,
	                         AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp,
	                         int32 OtherBodyIndex);

	UFUNCTION()
	void HandleDoorStateChanged(UDoorStateControllerComponent* Component,
	                            const EDoorState NewState,
	                            const bool bChangedImmediately);
protected:
#if WITH_EDITOR
	virtual void UpdateDebugText() override;
#endif
};
