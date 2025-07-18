// 

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HDAPlayerWeaponAnimComponent.generated.h"


class AHDAPlayerWeaponBase;

UCLASS(ClassGroup=(Player), PrioritizeCategories=("Shake, Recoil"))
class HONKDUCKAGES_API UHDAPlayerWeaponAnimComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UHDAPlayerWeaponAnimComponent();

	virtual void TickComponent(float DeltaTime,
	                           ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void InitializeComponent() override;

public:
	UFUNCTION(BlueprintPure)
	AHDAPlayerWeaponBase* GetOwningWeapon() const { return OwningWeapon.Get(); }

protected:
	UPROPERTY(VisibleInstanceOnly, Category="Default")
	TWeakObjectPtr<AHDAPlayerWeaponBase> OwningWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shake")
	FVector ShakeLocationThreshold = FVector(2.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shake")
	FRotator ShakeRotationThreshold = FRotator(2.f);

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Shake",
		meta=(ClampMin=0, UIMin=0, Delta=1))
	float ShakeFadeSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Shake",
		meta=(ClampMin=0, UIMin=0, Delta=1))
	float ShakeFadeSpeedOnStopShooting = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shake")
	UCurveFloat* ShakeStrengthCurve = nullptr;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Shake",
		meta=(ClampMin=0, UIMin=0, ClampMax=1, UIMax=1, Delta=0.1))
	float ShakePerShot = 1.0;

	float ShakeStrength = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil")
	FVector RecoilLocationThreshold = FVector(5.f, 0.f, 0.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil")
	FRotator RecoilRotationThreshold = FRotator(15.f, 0.f, 0.f);

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Recoil",
		meta=(ClampMin=0, UIMin=0, ClampMax=1, UIMax=1, Delta=0.1))
	float RecoilShotDelayThreshold = 0.5f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Recoil",
		meta=(ClampMin=0, UIMin=0, Delta=1))
	float RecoilFadeSpeed = 12.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil")
	UCurveFloat* RecoilFadeCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil")
	UCurveFloat* RecoilStrengthCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Recoil")
	bool bRecoilEachShotOnFullAuto = true;

	float RecoilStrength = 0.f;

	float RecoilTargetStrength = 0.f;

private:
	FVector InitialLocation = FVector::ZeroVector;

	FRotator InitialRotation = FRotator::ZeroRotator;
	
	void AddShake(const float Value);
	
	void CalculateShakeOffset(const float DeltaTime, FVector& OutLocation, FRotator& OutRotation);

	float GetNormalizedRemainingTime() const;

	float GetNormalizedElapsedTime() const;

	void CalculateRecoilOffset(const float DeltaTime, FVector& OutLocation, FRotator& OutRotation);

	UFUNCTION()
	void HandleWeaponShot(AHDAPlayerWeaponBase* Weapon);
};
