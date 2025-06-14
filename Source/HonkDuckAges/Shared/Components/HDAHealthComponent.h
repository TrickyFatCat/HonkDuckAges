// 

#pragma once

#include "CoreMinimal.h"
#include "TrickyPropertiesLibrary.h"
#include "Components/ActorComponent.h"
#include "HDAHealthComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHealthComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedDynamicSignature,
                                               UHDAHealthComponent*, Component,
                                               const FTrickyPropertyInt&, CurrentHealth,
                                               float, NormalizedHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthReachedZeroDynamicSignature,
                                            UHDAHealthComponent*, Component);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class HONKDUCKAGES_API UHDAHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHDAHealthComponent();

protected:
	virtual void InitializeComponent() override;

public:
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChangedDynamicSignature OnHealthIncreased;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChangedDynamicSignature OnHealthDecreased;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChangedDynamicSignature OnMaxHealthIncreased;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthReachedZeroDynamicSignature OnHealthReachedZero;

	UPROPERTY(EditDefaultsOnly,
		Category="Health",
		meta=(ClampMin=1, UIMin=1))
	int32 DefaultHealth = 100;

	UFUNCTION(BlueprintGetter, Category="Health")
	FTrickyPropertyInt GetHealth() const { return CurrentHealth; };

	UFUNCTION(BlueprintCallable, Category="Health")
	bool IncreaseHealth(const int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category="Health")
	bool DecreaseHealth(const int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category="Health")
	bool IncreaseMaxHealth(const int32 Amount = 1);

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetHealth, Category="Health")
	FTrickyPropertyInt CurrentHealth{0, 100, 100};

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	static void PrintLog(const FString& Message);

	static void PrintWarning(const FString& Message);

	static void PrintError(const FString& Message);
#endif
};
