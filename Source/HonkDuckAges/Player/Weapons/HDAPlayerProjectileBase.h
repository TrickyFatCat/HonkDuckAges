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
	virtual void PostInitializeComponents() override;

public:
	void InitProjectile(const FHitResult& HitResult, const int32 NewDamage);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	bool bDestroyOnHit = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Projectile")
	int32 Damage = 10;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> CollisionComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent = nullptr;

private:
	UFUNCTION()
	void HandleHit(UPrimitiveComponent* HitComponent,
	               AActor* OtherActor,
	               UPrimitiveComponent* OtherComp,
	               FVector NormalImpulse,
	               const FHitResult& Hit);

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;
#endif
};
