// 

#pragma once

#include "CoreMinimal.h"
#include "HDAWeaponStateController.h"
#include "GameFramework/Actor.h"
#include "HDAPlayerWeaponBase.generated.h"

struct FTrickyPropertyInt;
enum class EWeaponAmmoType : uint8;
class UHDAPlayerWeaponManager;
enum class EWeaponState : uint8;
class UHDAWeaponStateController;
class AHDAPlayerProjectileBase;
class UArrowComponent;

UENUM(BlueprintType)
enum class EWeaponBulletType : uint8
{
	Projectile,
	Trace
};

UENUM(BlueprintType)
enum class EWeaponMode : uint8
{
	FullAuto,
	SemiAuto
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerWeaponShotDynamicSignature,
                                            AHDAPlayerWeaponBase*, Weapon);

UCLASS(PrioritizeCategories="Weapon")
class HONKDUCKAGES_API AHDAPlayerWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AHDAPlayerWeaponBase();

protected:
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerWeaponShotDynamicSignature OnPlayerWeaponShot;

	virtual void StartShooting();

	virtual void StopShooting();

	void ActivateWeapon();

	void DeactivateWeapon();

	UFUNCTION(BlueprintPure)
	EWeaponState GetCurrentState() const { return WeaponStateController->GetCurrentState(); }

	UFUNCTION(BlueprintGetter)
	EWeaponBulletType GetBulletType() const { return BulletType; }

	UFUNCTION(BlueprintGetter)
	TSubclassOf<AHDAPlayerProjectileBase> GetProjectileClass() const { return ProjectileClass; }

	UFUNCTION(BlueprintGetter)
	EWeaponMode GetWeaponMode() const { return WeaponMode; }

	UFUNCTION()
	void SetOwningWeaponManager(UHDAPlayerWeaponManager* NewManager);

	UFUNCTION(BlueprintGetter)
	float GetSpreadAngle() const { return SpreadAngleDeg; }

	UFUNCTION(BlueprintGetter)
	float GetSpreadRadius() const { return SpreadRadius; }

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UMeshComponent> MeshComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHDAWeaponStateController> WeaponStateController = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UHDAPlayerWeaponManager> OwningWeaponManager = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter=GetBulletType, Category="Weapon")
	EWeaponBulletType BulletType = EWeaponBulletType::Projectile;

	UPROPERTY(EditDefaultsOnly,
		BlueprintGetter=GetProjectileClass,
		Category="Weapon",
		meta=(EditCondition="BulletType == EWeaponBulletType::Projectile", EditConditionHides))
	TSubclassOf<AHDAPlayerProjectileBase> ProjectileClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly,
		Category="Weapon",
		meta=(EditCondition="BulletType == EWeaponBulletType::Projectile", EditConditionHides))
	FName SpawnSocketName = TEXT("ProjectileSpawn");

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Weapon",
		meta=(ClampMin=1, UIMin=1, Delta=100, ForceUnits="Centimeters"))
	float TraceLength = 10000.f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Weapon",
		meta=(ClampMin=1, UIMin=1, ClampMax=99, UIMax=99));
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Weapon",
		meta=(ClampMin=1, UIMin=1))
	int32 Damage = 10;

	UPROPERTY(EditDefaultsOnly,
		BlueprintGetter=GetWeaponMode,
		Category="Weapon")
	EWeaponMode WeaponMode = EWeaponMode::FullAuto;

	UPROPERTY(EditDefaultsOnly,
		BlueprintGetter=GetSpreadAngle,
		Category="Weapon",
		meta=(ClampMin=0, UIMin=0, ClampMax=30, UIMax=30, Delta=1, ForceUnits="Degrees"))
	float SpreadAngleDeg = 5.f;

	UPROPERTY(VisibleInstanceOnly,
		BlueprintGetter=GetSpreadRadius,
		Category="Weapon")
	float SpreadRadius = 0.f;

	UPROPERTY(EditDefaultsOnly,
		Category="Weapon")
	UCurveFloat* SpreadDistribution = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> ForwardVector = nullptr;
#endif

private:
	void CalculateBulletDisplacement(FVector2D& Displacement) const;

	void MakeShot();

	AHDAPlayerProjectileBase* SpawnProjectile(const FVector& Direction);

	UFUNCTION()
	void HandleAmmoIncreased(UHDAPlayerWeaponManager* Component,
	                         EWeaponAmmoType AmmoType,
	                         const FTrickyPropertyInt& Ammo,
	                         int32 DeltaValue);

	void SetWeaponEnabled(const bool bIsEnabled);

	UFUNCTION()
	void HandleWeaponStateChanged(UHDAWeaponStateController* Component,
	                              EWeaponState NewState,
	                              bool bTransitImmediately);
};
