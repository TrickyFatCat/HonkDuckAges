#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HDAPlayerCharacter.generated.h"

class UHDAPlayerWeaponManager;
class UKeyringComponent;
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintPure, Category="Movement")
	FVector GetLateralVelocity() const;

	UFUNCTION(BlueprintPure, Category="Movement")
	float GetLateralSpeed() const;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* ShootAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* ChooseWeaponAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* LastWeaponAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* NextWeaponAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inputs")
	UInputAction* PreviousWeaponAction = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHDALifeStateComponent> LifeStateComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStatusEffectsManagerComponent> StatusEffectsManager = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UKeyringComponent> KeyringComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHDAPlayerWeaponManager> WeaponManagerComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UHDAPlayerMovementComponent> PlayerMovementComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	TSubclassOf<UStatusEffectBase> DashInvulnerabilityEffect = nullptr;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Movement",
		meta=(ClampMin=0, UIMin=0, ClampMax=10, UIMax=10, Delta=1, ForceUnits="Degrees"));
	float CameraLeanAngle = 3.0f;

	UPROPERTY(EditDefaultsOnly,
		BlueprintReadOnly,
		Category="Movement",
		meta=(ClampMin=0, UIMin=0))
	float CameraLeanSpeed = 10.0f;

private:
	FVector MovementDirection = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController = nullptr;

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

	UFUNCTION()
	void StartShooting();

	UFUNCTION()
	void StopShooting();

	UFUNCTION()
	void ChooseWeapon(const FInputActionValue& Value);

	UFUNCTION()
	void ChooseLastWeapon();

	UFUNCTION()
	void ChooseNextWeapon();

	UFUNCTION()
	void ChoosePreviousWeapon();

	UFUNCTION()
	void HandleZeroHealth(UHDALifeStateComponent* Component);

	void ProcessCameraLean(const float DeltaTime) const;

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	void RegisterConsoleCommands();

	static void UnregisterConsoleCommands();

	bool bShowDebugData = false;

	void TogglePlayerDebugData();

	void PrintPlayerDebugData(const float DeltaTime) const;

	void ToggleGodMode();

	void PrintGodModeMessage(const float DeltaTime) const;

	bool bIsDemigod = false;

	void ToggleDemigodMode();

	void PrintDemiGodMessage(const float DeltaTime) const;

	void GiveAllWeapons() const;

	void GiveAllAmmo() const;
#endif
};
