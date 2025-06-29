// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDAPlayerWeaponBase.generated.h"

class UArrowComponent;

UCLASS(PrioritizeCategories="Weapon")
class HONKDUCKAGES_API AHDAPlayerWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AHDAPlayerWeaponBase();

public:
	virtual void StartShooting();

	virtual void StopShooting();

	virtual void MakeShot();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;
#endif
};
