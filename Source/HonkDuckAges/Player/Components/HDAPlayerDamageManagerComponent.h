// 

#pragma once

#include "CoreMinimal.h"
#include "HonkDuckAges/Shared/Components/HDALifeStateComponent.h"
#include "HDAPlayerDamageManagerComponent.generated.h"


class UHDAArmorComponent;

UCLASS(ClassGroup=(Player))
class HONKDUCKAGES_API UHDAPlayerDamageManagerComponent : public UHDALifeStateComponent
{
	GENERATED_BODY()

public:
	UHDAPlayerDamageManagerComponent();

protected:
	virtual void InitializeComponent() override;

	UPROPERTY()
	TObjectPtr<UHDAArmorComponent> ArmorComponent = nullptr;

private:
};
