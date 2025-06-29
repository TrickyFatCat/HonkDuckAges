// 

#pragma once

#include "CoreMinimal.h"
#include "HDAWeaponInterface.h"
#include "GameFramework/Actor.h"
#include "HDAWeaponBase.generated.h"

class UArrowComponent;

UCLASS(PrioritizeCategories="Weapon")
class HONKDUCKAGES_API AHDAWeaponBase : public AActor, public IHDAWeaponInterface
{
	GENERATED_BODY()

public:
	AHDAWeaponBase();

public:
	virtual void StartShooting() override;

	virtual void StopShooting() override;

	virtual void MakeShot() override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;
#endif
};
