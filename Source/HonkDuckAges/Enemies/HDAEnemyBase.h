// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HDAEnemyBase.generated.h"

class UHDALifeStateComponent;

UCLASS()
class HONKDUCKAGES_API AHDAEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AHDAEnemyBase();

protected:
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHDALifeStateComponent> LifeStateComponent = nullptr;
};
