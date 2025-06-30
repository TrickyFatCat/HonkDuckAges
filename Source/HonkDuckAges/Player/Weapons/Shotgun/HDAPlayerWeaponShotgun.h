// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Player/Weapons/Core/HDAPlayerWeaponBase.h"
#include "HDAPlayerWeaponShotgun.generated.h"

UCLASS(Abstract, HideDropdown)
class HONKDUCKAGES_API AHDAPlayerWeaponShotgun : public AHDAPlayerWeaponBase
{
	GENERATED_BODY()

public:
	AHDAPlayerWeaponShotgun();
};
