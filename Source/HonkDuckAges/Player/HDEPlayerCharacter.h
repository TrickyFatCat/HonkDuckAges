
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HDEPlayerCharacter.generated.h"

UCLASS(Abstract)
class HONKDUCKAGES_API AHDEPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHDEPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
