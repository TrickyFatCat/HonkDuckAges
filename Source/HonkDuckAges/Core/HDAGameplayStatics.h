// 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HDAGameplayStatics.generated.h"

/**
 * 
 */
UCLASS()
class HONKDUCKAGES_API UHDAGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Heal")
	static bool ApplyHeal(AActor* Target, const int32 HealPower);

	UFUNCTION(BlueprintCallable, Category="Heal")
	static bool RestoreArmor(AActor* Target, const int32 Value);

};
