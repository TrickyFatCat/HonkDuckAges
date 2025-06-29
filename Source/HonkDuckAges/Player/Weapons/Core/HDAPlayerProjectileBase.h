// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDAPlayerProjectileBase.generated.h"

class UArrowComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS(PrioritizeCategories="Projectile")
class HONKDUCKAGES_API AHDAPlayerProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AHDAPlayerProjectileBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile", meta=(ExposeOnSpawn))
	int32 Damage = 10;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> CollisionComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;
#endif
};
