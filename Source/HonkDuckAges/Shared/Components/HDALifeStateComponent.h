// 

#pragma once

#include "CoreMinimal.h"
#include "TrickyPropertiesLibrary.h"
#include "Components/ActorComponent.h"
#include "HDALifeStateComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLifeStateComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvulnerabilityChangedDynamicSignature,
                                            UHDALifeStateComponent*, Component);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPropertyChangedDynamicSignature,
                                               UHDALifeStateComponent*, Component,
                                               const FTrickyPropertyInt&, Property,
                                               float, NormalizedValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPropertyReachedZeroDynamicSignature,
                                            UHDALifeStateComponent*, Component);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class HONKDUCKAGES_API UHDALifeStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHDALifeStateComponent();

protected:
	virtual void InitializeComponent() override;

public:
	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnInvulnerabilityChangedDynamicSignature OnInvulnerabilityEnabled;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnInvulnerabilityChangedDynamicSignature OnInvulnerabilityDisabled;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyChangedDynamicSignature OnHealthIncreased;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyChangedDynamicSignature OnHealthDecreased;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyChangedDynamicSignature OnMaxHealthIncreased;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyReachedZeroDynamicSignature OnHealthReachedZero;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyChangedDynamicSignature OnArmorIncreased;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyChangedDynamicSignature OnArmorDecreased;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyChangedDynamicSignature OnMaxArmorIncreased;

	UPROPERTY(BlueprintAssignable, Category="LifeState")
	FOnPropertyReachedZeroDynamicSignature OnArmorReachedZero;

	UPROPERTY(EditDefaultsOnly,
		Category="LifeState",
		meta=(ClampMin=1, UIMin=1))
	int32 DefaultHealth = 100;

	UPROPERTY(EditDefaultsOnly,
		Category="LifeState",
		meta=(ClampMin=1, UIMin=1))
	int32 DefaultArmor = 0;

	UFUNCTION(BlueprintCallable, Category="LifeState")
	bool IncreaseHealth(const int32 Value);

	UFUNCTION(BlueprintCallable, Category="LifeState")
	bool IncreaseMaxHealth(const int32 Value);

	UFUNCTION(BlueprintCallable, Category="LifeState")
	bool IncreaseArmor(const int32 Value);

	UFUNCTION(BlueprintCallable, Category="LifeState")
	bool IncreaseMaxArmor(const int32 Value);
	
	UFUNCTION(BlueprintGetter, Category="LifeState")
	bool GetIsInvulnerable() const { return bIsInvulnerable; };

	UFUNCTION(BlueprintSetter, Category="LifeState")
	void SetIsInvulnerable(const bool Value);

	UFUNCTION(BlueprintGetter, Category="LifeState")
	FTrickyPropertyInt GetHealth() const { return Health; }

	UFUNCTION(BlueprintGetter, Category="LifeState")
	FTrickyPropertyInt GetArmor() const { return Armor; }

private:
	UPROPERTY(VisibleInstanceOnly,
		BlueprintGetter=GetIsInvulnerable,
		BlueprintSetter=SetIsInvulnerable,
		Category="LifeState")
	bool bIsInvulnerable = false;

	UPROPERTY(VisibleInstanceOnly,
		BlueprintGetter=GetHealth,
		Category="LifeState")
	FTrickyPropertyInt Health;

	UPROPERTY(VisibleInstanceOnly,
		BlueprintGetter=GetArmor,
		Category="LifeState")
	FTrickyPropertyInt Armor;

	UFUNCTION()
	void HandleDamageTaken(AActor* DamagedActor,
	                       float Damage,
	                       const UDamageType* DamageType,
	                       AController* InstigatedBy,
	                       AActor* DamageCauser);

	void DecreaseArmor(const int32 Value);

	void DecreaseHealth(const int32 Value);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	static void PrintLog(const FString& Message);

	static void PrintWarning(const FString& Message);

	static void PrintError(const FString& Message);
#endif
};
