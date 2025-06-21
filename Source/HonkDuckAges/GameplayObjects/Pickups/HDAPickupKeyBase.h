// 

#pragma once

#include "CoreMinimal.h"
#include "Pickup/PickupAutomaticBase.h"
#include "HDAPickupKeyBase.generated.h"

class UKeyringComponent;
class ULockKeyType;

UCLASS()
class HONKDUCKAGES_API AHDAPickupKeyBase : public APickupAutomaticBase
{
	GENERATED_BODY()

public:
	AHDAPickupKeyBase();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup")
	TSubclassOf<ULockKeyType> KeyClass = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UKeyringComponent> KeyringComponent = nullptr;

	virtual bool CanBeActivated_Implementation(AActor* Activator) override;

	virtual void HandleActivationSuccess_Implementation(AActor* Activator) override;
};
