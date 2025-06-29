// 


#include "HDAPlayerWeaponManager.h"


UHDAPlayerWeaponManager::UHDAPlayerWeaponManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UHDAPlayerWeaponManager::InitializeComponent()
{
	Super::InitializeComponent();
}

void UHDAPlayerWeaponManager::StartShooting()
{
}

void UHDAPlayerWeaponManager::StopShooting()
{
}

void UHDAPlayerWeaponManager::AddWeapon(const EWeaponSlot WeaponSlot)
{
}

void UHDAPlayerWeaponManager::ChooseWeapon(const EWeaponSlot WeaponSlot)
{
}

void UHDAPlayerWeaponManager::ChoosePreviousWeapon()
{
}
