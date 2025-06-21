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
};
