// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponBase.h"
#include "HDAPlayerWeaponChain.generated.h"

UCLASS(Abstract, HideDropdown)
class HONKDUCKAGES_API AHDAPlayerWeaponChain : public AHDAPlayerWeaponBase
{
	GENERATED_BODY()

public:
	AHDAPlayerWeaponChain();
};
