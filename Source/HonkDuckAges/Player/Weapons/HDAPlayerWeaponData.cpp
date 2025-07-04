// 


#include "HDAPlayerWeaponData.h"

FString UHDAPlayerWeaponData::GetSlotName(const EWeaponSlot WeaponSlot)
{
	return StaticEnum<EWeaponSlot>()->GetNameStringByValue(static_cast<int64>(WeaponSlot));
}

FString UHDAPlayerWeaponData::GetAmmoTypeName(const EWeaponAmmoType AmmoType)
{
	return StaticEnum<EWeaponAmmoType>()->GetNameStringByValue(static_cast<int64>(AmmoType));
}
