// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Player/Weapons/HDAPlayerWeaponBase.h"
#include "HDAPlayerWeaponSlug.generated.h"

UCLASS(Abstract, HideDropdown)
class HONKDUCKAGES_API AHDAPlayerWeaponSlug : public AHDAPlayerWeaponBase
{
	GENERATED_BODY()

public:
	AHDAPlayerWeaponSlug();
};
