
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HDAPlayerCharacter.generated.h"

class UHDAArmorComponent;
class UHDAHealthComponent;

UCLASS(Abstract)
class HONKDUCKAGES_API AHDAPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHDAPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHDAHealthComponent> HealthComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHDAArmorComponent> ArmorComponent = nullptr;
};
