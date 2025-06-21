// 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HDAGameplayStatics.generated.h"

class ULockKeyType;
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

	UFUNCTION(BlueprintCallable, Category="Keys")
	static bool CanGiveKey(AActor* Target, TSubclassOf<ULockKeyType> KeyClass);

	UFUNCTION(BlueprintCallable, Category="Keys")
	static void GiveKey(AActor* Target, TSubclassOf<ULockKeyType> KeyClass);
};
