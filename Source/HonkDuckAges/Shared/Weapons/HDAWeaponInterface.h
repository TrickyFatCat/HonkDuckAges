// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HDAWeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHDAWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HONKDUCKAGES_API IHDAWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual void StartShooting() = 0;

	UFUNCTION()
	virtual void StopShooting() = 0;

	UFUNCTION()
	virtual void MakeShot() = 0;
};
