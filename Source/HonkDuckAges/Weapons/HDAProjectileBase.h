// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDAProjectileBase.generated.h"

class UArrowComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS(PrioritizeCategories="Projectile")
class HONKDUCKAGES_API AHDAProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AHDAProjectileBase();

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
