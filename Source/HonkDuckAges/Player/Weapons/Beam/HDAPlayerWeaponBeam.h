// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Player/Weapons/Core/HDAPlayerWeaponBase.h"
#include "HDAPlayerWeaponBeam.generated.h"

UCLASS(Abstract, HideDropdown)
class HONKDUCKAGES_API AHDAPlayerWeaponBeam : public AHDAPlayerWeaponBase
{
	GENERATED_BODY()

public:
	AHDAPlayerWeaponBeam();
};
