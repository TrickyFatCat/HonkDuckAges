// 

#pragma once

#include "CoreMinimal.h"
#include "Door/DoorInterface.h"
#include "GameFramework/Actor.h"
#include "HDADoorBase.generated.h"

class UArrowComponent;
class ULockKeyType;
class ULockStateControllerComponent;
class UDoorStateControllerComponent;

UCLASS()
class HONKDUCKAGES_API AHDADoorBase : public AActor, public IDoorInterface
{
	GENERATED_BODY()

public:
	AHDADoorBase();

protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void PostInitializeComponents() override;

public:
	virtual bool OpenDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool CloseDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool UnlockDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool LockDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool DisableDoor_Implementation(const bool bTransitImmediately) override;

	virtual bool ReverseDoorStateTransition_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Door")
	EDoorState InitialState = EDoorState::Closed;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Door",
		meta=(EditCondition="InitialState == EDoorState::Locked"))
	TSubclassOf<ULockKeyType> RequiredKey = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UDoorStateControllerComponent> DoorStateControllerComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<ULockStateControllerComponent> LockStateControllerComponent = nullptr;

private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;
	
#endif
	UFUNCTION()
	void HandleLockStateChanged(ULockStateControllerComponent* Component,
	                            ELockState NewState,
	                            bool bChangedImmediately);
};

