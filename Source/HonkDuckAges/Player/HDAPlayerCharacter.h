
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HDAPlayerCharacter.generated.h"

class UHDALifeStateComponent;
class UStatusEffectBase;
class UStatusEffectsManagerComponent;
class UHDAPlayerMovementComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;

UCLASS(Abstract)
class HONKDUCKAGES_API AHDAPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHDAPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputMappingContext* DefaultMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* MoveAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* AimAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* DashAction = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHDALifeStateComponent> LifeStateComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStatusEffectsManagerComponent> StatusEffectsManager = nullptr;

	UPROPERTY()
	TObjectPtr<UHDAPlayerMovementComponent> PlayerMovementComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UStatusEffectBase> DashInvulnerabilityEffect = nullptr;

private:
	FVector MovementDirection = FVector::ZeroVector;
	
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void StopMoving(const FInputActionValue& Value);

	UFUNCTION()
	void Aim(const FInputActionValue& Value);

	UFUNCTION()
	void Dash();

	UFUNCTION()
	void HandleDashStarted();

	UFUNCTION()
	void HandleDashFinished();

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	bool bShowDebugData = false;
	
	void TogglePlayerDebugData();
#endif
};
