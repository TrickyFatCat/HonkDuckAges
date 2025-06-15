// 

#pragma once

#include "CoreMinimal.h"
#include "TrickyPropertiesLibrary.h"
#include "Components/ActorComponent.h"
#include "HDAArmorComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArmorComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnArmorChangedDynamicSignature,
                                               UHDAArmorComponent*, Component,
                                               const FTrickyPropertyInt&, CurrentArmor,
                                               float, NormalizedArmor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorReachedZeroDynamicSignature,
                                            UHDAArmorComponent*, Component);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class HONKDUCKAGES_API UHDAArmorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHDAArmorComponent();

protected:
	virtual void InitializeComponent() override;

	UPROPERTY(BlueprintAssignable, Category="Armor")
	FOnArmorChangedDynamicSignature OnArmorIncreased;

	UPROPERTY(BlueprintAssignable, Category="Armor")
	FOnArmorChangedDynamicSignature OnArmorDecreased;

	UPROPERTY(BlueprintAssignable, Category="Armor")
	FOnArmorChangedDynamicSignature OnMaxArmorIncreased;

	UPROPERTY(BlueprintAssignable, Category="Armor")
	FOnArmorReachedZeroDynamicSignature OnArmorReachedZero;

	UPROPERTY(EditDefaultsOnly,
		Category="Armor",
		meta=(ClampMin=1, UIMin=1))
	int32 DefaultArmor = 100;

	UFUNCTION(BlueprintGetter, Category="Armor")
	FTrickyPropertyInt GetArmor() const { return CurrentArmor; };

	UFUNCTION(BlueprintCallable, Category="Armor")
	bool IncreaseArmor(const int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category="Armor")
	bool DecreaseArmor(const int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category="Armor")
	bool IncreaseMaxArmor(const int32 Amount = 1);

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintGetter=GetArmor, Category="Armor")
	FTrickyPropertyInt CurrentArmor{0, 100, 100};

#if WITH_EDITOR && !UE_BUILD_SHIPPING
	static void PrintLog(const FString& Message);

	static void PrintWarning(const FString& Message);

	static void PrintError(const FString& Message);
#endif
};
