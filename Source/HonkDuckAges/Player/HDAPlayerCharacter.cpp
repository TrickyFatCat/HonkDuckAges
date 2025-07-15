#include "HDAPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "StatusEffectsManagerComponent.h"
#include "Components/HDAPlayerMovementComponent.h"
#include "Components/HDAPlayerWeaponManager.h"
#include "HonkDuckAges/Shared/Components/HDALifeStateComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LockKey/KeyringComponent.h"
#include "LockKey/LockKeyType.h"
#include "Weapons/HDAPlayerWeaponBase.h"


AHDAPlayerCharacter::AHDAPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UHDAPlayerMovementComponent>(TEXT("CharMoveComp")))
{
	PrimaryActorTick.bCanEverTick = true;

	LifeStateComponent = CreateDefaultSubobject<UHDALifeStateComponent>(TEXT("LifeStateComponent"));
	LifeStateComponent->DefaultArmor = 25;
	StatusEffectsManager = CreateDefaultSubobject<UStatusEffectsManagerComponent>(TEXT("StatusEffectsManager"));
	KeyringComponent = CreateDefaultSubobject<UKeyringComponent>(TEXT("KeyringComponent"));
	WeaponManagerComponent = CreateDefaultSubobject<UHDAPlayerWeaponManager>(TEXT("WeaponManagerComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));
	CameraComponent->bUsePawnControlRotation = true;
}

void AHDAPlayerCharacter::BeginPlay()
{
	if (IsValid(PlayerController))
	{
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (EnhancedInputSubsystem)
		{
			EnhancedInputSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	WeaponInitialLocation = WeaponManagerComponent->GetWeaponData()->SpawnPositionOffset;
	PlayerMovementComponent = Cast<UHDAPlayerMovementComponent>(GetMovementComponent());

	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->OnDashStarted.AddDynamic(this, &AHDAPlayerCharacter::HandleDashStarted);
		PlayerMovementComponent->OnDashFinished.AddDynamic(this, &AHDAPlayerCharacter::HandleDashFinished);
	}

	LifeStateComponent->OnHealthReachedZero.AddUniqueDynamic(this, &AHDAPlayerCharacter::HandleZeroHealth);

	ensureMsgf(PlayerMovementComponent != nullptr,
	           TEXT("%s movement component isn't changed to UHDAPlayerMovementComponent"),
	           *GetActorNameOrLabel());

	ensureMsgf(DefaultMappingContext != nullptr, TEXT("DefaultMappingContext wasn't set for %s"),
	           *GetActorNameOrLabel());

	ensureMsgf(MoveAction != nullptr, TEXT("MoveAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(AimAction != nullptr, TEXT("AimAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(JumpAction != nullptr, TEXT("JumpAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(DashAction != nullptr, TEXT("DashAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(ShootAction != nullptr, TEXT("ShootAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(ChooseWeaponAction != nullptr, TEXT("ChooseWeaponAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(LastWeaponAction != nullptr, TEXT("LastWeaponAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(NextWeaponAction != nullptr, TEXT("NextWeaponAction wasn't set for %s"), *GetActorNameOrLabel());

	ensureMsgf(PreviousWeaponAction != nullptr, TEXT("PreviousWeaponAction wasn't set for %s"), *GetActorNameOrLabel());

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	RegisterConsoleCommands();
#endif

	Super::BeginPlay();
}

void AHDAPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	UnregisterConsoleCommands();
#endif
}

void AHDAPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ProcessCameraLean(DeltaTime);
	AnimateRotationSway(DeltaTime);
	AnimateLocationSway(DeltaTime);

#if WITH_EDITOR || !UE_BUILD_SHIPPING
	PrintPlayerDebugData(DeltaTime);
	PrintGodModeMessage(DeltaTime);
	PrintDemiGodMessage(DeltaTime);
#endif
}

void AHDAPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHDAPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction,
		                                   ETriggerEvent::Completed,
		                                   this,
		                                   &AHDAPlayerCharacter::StopMoving);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AHDAPlayerCharacter::Aim);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHDAPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction,
		                                   ETriggerEvent::Completed,
		                                   this,
		                                   &AHDAPlayerCharacter::StopJumping);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AHDAPlayerCharacter::Dash);
		EnhancedInputComponent->BindAction(ShootAction,
		                                   ETriggerEvent::Started,
		                                   this,
		                                   &AHDAPlayerCharacter::StartShooting);
		EnhancedInputComponent->BindAction(ShootAction,
		                                   ETriggerEvent::Completed,
		                                   this,
		                                   &AHDAPlayerCharacter::StopShooting);
		EnhancedInputComponent->BindAction(ChooseWeaponAction,
		                                   ETriggerEvent::Started,
		                                   this,
		                                   &AHDAPlayerCharacter::ChooseWeapon);
		EnhancedInputComponent->BindAction(LastWeaponAction,
		                                   ETriggerEvent::Started,
		                                   this,
		                                   &AHDAPlayerCharacter::ChooseLastWeapon);
		EnhancedInputComponent->BindAction(NextWeaponAction,
		                                   ETriggerEvent::Started,
		                                   this,
		                                   &AHDAPlayerCharacter::ChooseNextWeapon);
		EnhancedInputComponent->BindAction(PreviousWeaponAction,
		                                   ETriggerEvent::Started,
		                                   this,
		                                   &AHDAPlayerCharacter::ChoosePreviousWeapon);
	}
}

void AHDAPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<APlayerController>(NewController);
}

FVector AHDAPlayerCharacter::GetLateralVelocity() const
{
	return PlayerMovementComponent->GetLateralVelocity();
}

float AHDAPlayerCharacter::GetLateralSpeed() const
{
	return PlayerMovementComponent->GetLateralSpeed();
}

void AHDAPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D InputDirection = Value.Get<FVector2D>();
	MovementDirection = FVector(InputDirection.X, InputDirection.Y, 0.f);

	FVector ForwardDirection = GetActorForwardVector();
	ForwardDirection.Z = 0.f;
	ForwardDirection.Normalize();

	FVector RightDirection = GetActorRightVector();
	RightDirection.Z = 0.f;
	RightDirection.Normalize();

	AddMovementInput(ForwardDirection, MovementDirection.X);
	AddMovementInput(RightDirection, MovementDirection.Y);
}

void AHDAPlayerCharacter::StopMoving(const FInputActionValue& Value)
{
	MovementDirection = FVector::ZeroVector;
}

void AHDAPlayerCharacter::Aim(const FInputActionValue& Value)
{
	const FVector2D AimDirection = Value.Get<FVector2D>();
	AddControllerYawInput(AimDirection.X);
	AddControllerPitchInput(AimDirection.Y);
	CalculateTargetSwayRotation(AimDirection);
}

void AHDAPlayerCharacter::Dash()
{
	FVector ForwardDirection = GetActorForwardVector();
	ForwardDirection.Z = 0.f;
	ForwardDirection = ForwardDirection.GetSafeNormal();

	const FVector RightDirection = GetActorRightVector();
	FVector DashDirection = ForwardDirection;

	if (!MovementDirection.IsNearlyZero())
	{
		DashDirection = (ForwardDirection * MovementDirection.X + RightDirection * MovementDirection.Y).GetSafeNormal();
	}

	//It's important to call it next frame to keep consistent distance
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(PlayerMovementComponent, FName("StartDashing"), DashDirection);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}

void AHDAPlayerCharacter::HandleDashStarted()
{
	ensureMsgf(IsValid(DashInvulnerabilityEffect),
	           TEXT("DashInvulnerabilityEffect wasn't set for %s"),
	           *GetActorNameOrLabel());

	StatusEffectsManager->ApplyStatusEffect(DashInvulnerabilityEffect, this);
}

void AHDAPlayerCharacter::HandleDashFinished()
{
	ensureMsgf(IsValid(DashInvulnerabilityEffect),
	           TEXT("DashInvulnerabilityEffect wasn't set for %s"),
	           *GetActorNameOrLabel());

	StatusEffectsManager->RemoveStatusEffect(DashInvulnerabilityEffect, this);
}

void AHDAPlayerCharacter::StartShooting()
{
	if (!IsValid(WeaponManagerComponent))
	{
		return;
	}

	WeaponManagerComponent->StartShooting();
}

void AHDAPlayerCharacter::StopShooting()
{
	if (!IsValid(WeaponManagerComponent))
	{
		return;
	}

	WeaponManagerComponent->StopShooting();
}

void AHDAPlayerCharacter::ChooseWeapon(const FInputActionValue& Value)
{
	const int32 Index = static_cast<int32>(Value.Get<float>()) - 1;
	WeaponManagerComponent->ChooseWeaponByIndex(Index);
}

void AHDAPlayerCharacter::ChooseLastWeapon()
{
	WeaponManagerComponent->ChooseLastWeapon();
}

void AHDAPlayerCharacter::ChooseNextWeapon()
{
	WeaponManagerComponent->ChooseNextWeapon();
}

void AHDAPlayerCharacter::ChoosePreviousWeapon()
{
	WeaponManagerComponent->ChoosePreviousWeapon();
}

void AHDAPlayerCharacter::HandleZeroHealth(UHDALifeStateComponent* Component)
{
#if WITH_EDITOR || !UE_BUILD_SHIPPING
	if (bIsDemigod)
	{
		LifeStateComponent->IncreaseHealth(LifeStateComponent->GetHealth().MaxValue);
		LifeStateComponent->IncreaseArmor(LifeStateComponent->GetArmor().MaxValue);
		return;
	}
#endif
}

void AHDAPlayerCharacter::ProcessCameraLean(const float DeltaTime) const
{
	FRotator TargetRotation = PlayerController->GetControlRotation();
	TargetRotation.Roll = MovementDirection.Y != 0.f && !PlayerMovementComponent->IsFalling()
		                      ? CameraLeanAngle * MovementDirection.Y
		                      : 0.f;
	const FRotator CurrentRotation = PlayerController->GetControlRotation();
	const FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation,
	                                                           TargetRotation,
	                                                           DeltaTime,
	                                                           CameraLeanSpeed);
	PlayerController->SetControlRotation(NewRotation);
}

void AHDAPlayerCharacter::CalculateTargetSwayRotation(const FVector2D& Value)
{
	TargetSwayRotation.Roll = Value.X * RotationSwayPower.X;
	TargetSwayRotation.Pitch = -Value.Y * RotationSwayPower.Y;
	TargetSwayRotation.Yaw = Value.X * RotationSwayPower.Z;
}

void AHDAPlayerCharacter::AnimateRotationSway(const float DeltaTime) const
{
	AHDAPlayerWeaponBase* CurrentWeapon = WeaponManagerComponent->GetCurrentWeapon();

	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	const FRotator CurrentSwayRotation = CurrentWeapon->GetRootComponent()->GetRelativeRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentSwayRotation, TargetSwayRotation, DeltaTime, RotationSwaySpeed);
	NewRotation.Roll = FMath::Clamp(NewRotation.Roll, -RotationSwayThreshold.Roll, RotationSwayThreshold.Roll);
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -RotationSwayThreshold.Pitch, RotationSwayThreshold.Pitch);
	NewRotation.Yaw = FMath::Clamp(NewRotation.Yaw, -RotationSwayThreshold.Yaw, RotationSwayThreshold.Yaw);
	CurrentWeapon->SetActorRelativeRotation(NewRotation);
}

void AHDAPlayerCharacter::AnimateLocationSway(const float DeltaTime)
{
	FVector TargetLateralOffset = UKismetMathLibrary::Quat_UnrotateVector(GetActorRotation().Quaternion(),
	                                                                      GetLateralVelocity());
	TargetLateralOffset = TargetLateralOffset / PlayerMovementComponent->MaxWalkSpeed;
	TargetLateralOffset.X = FMath::Clamp(TargetLateralOffset.X, -1.f, 1.f);
	TargetLateralOffset.Y = FMath::Clamp(TargetLateralOffset.Y, -1.f, 1.f);
	CurrentLateralOffset = FMath::VInterpTo(CurrentLateralOffset, TargetLateralOffset, DeltaTime, LocationSwaySpeed);

	CurrentLateralOffset.X = FMath::Abs(CurrentLateralOffset.X) >= 0.1f ? CurrentLateralOffset.X : 0.f;
	CurrentLateralOffset.Y = FMath::Abs(CurrentLateralOffset.Y) >= 0.1f ? CurrentLateralOffset.Y : 0.f;

	FVector WeaponOffset = CurrentLateralOffset * LocationSwayThreshold;

	float TargetVerticalOffset = GetVelocity().Z / PlayerMovementComponent->GetJumpVelocity();
	TargetVerticalOffset = FMath::Clamp(TargetVerticalOffset, -1.0f, 1.0f);
	CurrentVerticalOffset = FMath::FInterpTo(CurrentVerticalOffset, TargetVerticalOffset, DeltaTime, LocationSwaySpeed);
	CurrentVerticalOffset = FMath::Abs(CurrentVerticalOffset) >= 0.1f ? CurrentVerticalOffset : 0.f;
	WeaponOffset.Z += CurrentVerticalOffset * LocationSwayThreshold.Z;

	const float NormalizedLateralSpeed = PlayerMovementComponent->GetNormalizedLateralSpeed();

	if (!PlayerMovementComponent->IsFalling() && NormalizedLateralSpeed >= 1.f)
	{
		const float Time = GetWorld()->GetTimeSeconds();
		const float MovementSin = FMath::Sin(NormalizedLateralSpeed * LocationSwayFrequency * Time);
		const float MovementSinSquared = MovementSin * MovementSin;

		WeaponOffset.X += LocationSwayAmplitude.X * MovementSinSquared;
		WeaponOffset.Y += LocationSwayAmplitude.Y * MovementSin;
		WeaponOffset.Z += LocationSwayAmplitude.Z * MovementSinSquared;
	}

	AHDAPlayerWeaponBase* CurrentWeapon = WeaponManagerComponent->GetCurrentWeapon();

	if (!IsValid(CurrentWeapon))
	{
		return;
	}

	const FVector CurrentLocation = CurrentWeapon->GetRootComponent()->GetRelativeLocation();
	const FVector TargetLocation = WeaponInitialLocation + WeaponOffset;
	const FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, LocationSwaySpeed);

	WeaponManagerComponent->GetCurrentWeapon()->SetActorRelativeLocation(NewLocation);
}

#if WITH_EDITOR || !UE_BUILD_SHIPPING

void AHDAPlayerCharacter::RegisterConsoleCommands()
{
	IConsoleManager::Get().RegisterConsoleCommand(TEXT("HDA.TogglePlayerDebugData"),
	                                              TEXT("Toggles debug data for player"),
	                                              FConsoleCommandDelegate::CreateUObject(
		                                              this, &AHDAPlayerCharacter::TogglePlayerDebugData));

	IConsoleManager::Get().RegisterConsoleCommand(TEXT("HDA.God"),
	                                              TEXT("Toggles god mode for player"),
	                                              FConsoleCommandDelegate::CreateUObject(
		                                              this, &AHDAPlayerCharacter::ToggleGodMode));

	IConsoleManager::Get().RegisterConsoleCommand(TEXT("HDA.Demigod"),
	                                              TEXT(
		                                              "Toggles demigod mode for player. Damage will be registered as usual, but health will fully restored on death"),
	                                              FConsoleCommandDelegate::CreateUObject(
		                                              this, &AHDAPlayerCharacter::ToggleDemigodMode));

	IConsoleManager::Get().RegisterConsoleCommand(TEXT("HDA.AllWeapons"),
	                                              TEXT("Gives all weapons to player"),
	                                              FConsoleCommandDelegate::CreateUObject(
		                                              this, &AHDAPlayerCharacter::GiveAllWeapons));

	IConsoleManager::Get().RegisterConsoleCommand(TEXT("HDA.AllAmmo"),
	                                              TEXT("Fully restores all ammo for player"),
	                                              FConsoleCommandDelegate::CreateUObject(
		                                              this, &AHDAPlayerCharacter::GiveAllAmmo));
}

void AHDAPlayerCharacter::UnregisterConsoleCommands()
{
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("HDA.TogglePlayerDebugData"), false);
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("HDA.God"), false);
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("HDA.Demigod"), false);
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("HDA.AllWeapons"), false);
	IConsoleManager::Get().UnregisterConsoleObject(TEXT("HDA.AllAmmo"), false);
}

void AHDAPlayerCharacter::TogglePlayerDebugData()
{
	bShowDebugData = !bShowDebugData;
}

void AHDAPlayerCharacter::PrintPlayerDebugData(const float DeltaTime) const
{
	if (!GEngine || !bShowDebugData)
	{
		return;
	}

	FString DebugMessage = "";

	DebugMessage = FString::Printf(TEXT("===KEYRING===\n"));
	TArray<TSubclassOf<ULockKeyType>> AcquiredLockKeys;
	IKeyringInterface::Execute_GetAcquiredLockKeys(KeyringComponent, AcquiredLockKeys);

	if (AcquiredLockKeys.IsEmpty())
	{
		DebugMessage = DebugMessage.Append(TEXT("EMPTY\n"));
	}
	else
	{
		for (const TSubclassOf<ULockKeyType>& LockKey : AcquiredLockKeys)
		{
			DebugMessage = DebugMessage.Appendf(TEXT("%s\n"), *LockKey->GetName());
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Cyan, DebugMessage);

	DebugMessage = FString::Printf(TEXT("===CURRENT WEAPON===\n"));

	AHDAPlayerWeaponBase* CurrentWeapon = WeaponManagerComponent->GetCurrentWeapon();
	DebugMessage = DebugMessage.Appendf(TEXT("%s\n"), *CurrentWeapon->GetActorNameOrLabel());
	const FString WeaponStateName = StaticEnum<EWeaponState>()->GetNameStringByValue(
		static_cast<int64>(CurrentWeapon->GetCurrentState()));
	DebugMessage = DebugMessage.Appendf(TEXT("Current State: %s\n"), *WeaponStateName);

	DebugMessage = DebugMessage.Append(TEXT("\n===ACQUIRED WEAPONS===\n"));
	TArray<AHDAPlayerWeaponBase*> AcquiredWeapons;
	WeaponManagerComponent->GetAcquiredWeapons(AcquiredWeapons);

	if (!AcquiredWeapons.IsEmpty())
	{
		for (auto Weapon : AcquiredWeapons)
		{
			if (!IsValid(Weapon))
			{
				continue;
			}

			DebugMessage = DebugMessage.Append(FString::Printf(TEXT("%s\n"), *Weapon->GetActorNameOrLabel()));
		}
	}
	else
	{
		DebugMessage = DebugMessage.Append(TEXT("NONE\n"));
	}

	TMap<EWeaponAmmoType, FTrickyPropertyInt> AmmoStash;
	WeaponManagerComponent->GetAmmoStash(AmmoStash);
	DebugMessage = DebugMessage.Append(TEXT("\n===AMMO===\n"));

	if (AmmoStash.IsEmpty())
	{
		DebugMessage = DebugMessage.Append(TEXT("NONE\n"));
	}
	else
	{
		for (const auto& [AmmoType, AmmoCount] : AmmoStash)
		{
			const FString AmmoTypeName = UHDAPlayerWeaponData::GetAmmoTypeName(AmmoType);
			DebugMessage = DebugMessage.Append(FString::Printf(TEXT("%s : %d / %d\n"),
			                                                   *AmmoTypeName,
			                                                   AmmoCount.Value,
			                                                   AmmoCount.MaxValue));
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Emerald, DebugMessage);

	DebugMessage = FString::Printf(TEXT("===PLAYER MOVEMENT===\n"));

	DebugMessage = DebugMessage.Append(FString::Printf(TEXT("Lateral Speed: %.2f\n"),
	                                                   GetLateralSpeed()));
	DebugMessage = DebugMessage.Append(FString::Printf(TEXT("Gravity Scale: %.2f\n"),
	                                                   PlayerMovementComponent->GravityScale));
	DebugMessage = DebugMessage.Append(FString::Printf(TEXT("Jump Charges: %d / %d\n"),
	                                                   JumpMaxCount - JumpCurrentCountPreJump,
	                                                   JumpMaxCount));
	DebugMessage = DebugMessage.Append(
		FString::Printf(TEXT("Dash Charges: %d / %d\n"),
		                PlayerMovementComponent->GetDashCharges(),
		                PlayerMovementComponent->GetDashMaxCharges()));
	DebugMessage = DebugMessage.Append(
		FString::Printf(TEXT("Cached Dash Charges: %d / %d\n"),
		                PlayerMovementComponent->GetCachedDashCharges(),
		                PlayerMovementComponent->GetDashMaxCharges()));
	DebugMessage = DebugMessage.
		Append(FString::Printf(TEXT("Dash Cooldown: %.2f"),
		                       PlayerMovementComponent->GetDashCooldownRemainingTime()));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, DebugMessage);

	DebugMessage = FString::Printf(TEXT("===PLAYER VITALS===\n"));
	const FString IsInvulnerable = LifeStateComponent->GetIsInvulnerable() ? TEXT("TRUE") : TEXT("FALSE");
	DebugMessage = DebugMessage.Append(FString::Printf(TEXT("Is Invulnerable: %s\n"),
	                                                   *IsInvulnerable));
	const FTrickyPropertyInt Health = LifeStateComponent->GetHealth();
	DebugMessage = DebugMessage.Append(FString::Printf(TEXT("Health: %d/%d | %.0f %%\n"),
	                                                   Health.Value,
	                                                   Health.MaxValue,
	                                                   Health.GetNormalizedValue() * 100));
	const FTrickyPropertyInt Armor = LifeStateComponent->GetArmor();
	DebugMessage = DebugMessage.Append(FString::Printf(TEXT("Armor: %d/%d | %.0f %%\n"),
	                                                   Armor.Value,
	                                                   Armor.MaxValue,
	                                                   Armor.GetNormalizedValue() * 100));

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Purple, DebugMessage);
}

void AHDAPlayerCharacter::ToggleGodMode()
{
	SetCanBeDamaged(!CanBeDamaged());

	if (!CanBeDamaged())
	{
		bIsDemigod = false;
	}
}

void AHDAPlayerCharacter::PrintGodModeMessage(const float DeltaTime) const
{
	if (!GEngine || CanBeDamaged())
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, "===GOD MODE ENABLED===", true, FVector2D(2));
}

void AHDAPlayerCharacter::ToggleDemigodMode()
{
	bIsDemigod = !bIsDemigod;

	if (bIsDemigod)
	{
		SetCanBeDamaged(true);
	}
}

void AHDAPlayerCharacter::PrintDemiGodMessage(const float DeltaTime) const
{
	if (!GEngine || !bIsDemigod)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, "===DEMIGOD MODE ENABLED===", true, FVector2D(2));
}

void AHDAPlayerCharacter::GiveAllWeapons() const
{
	WeaponManagerComponent->AddWeapon(EWeaponSlot::SlugShot);
	WeaponManagerComponent->AddWeapon(EWeaponSlot::HeavyChainGun);
	WeaponManagerComponent->AddWeapon(EWeaponSlot::MiniRockets);
	WeaponManagerComponent->AddWeapon(EWeaponSlot::PlasmaBeam);
	WeaponManagerComponent->AddWeapon(EWeaponSlot::LaserCrossbow);
	WeaponManagerComponent->AddWeapon(EWeaponSlot::ShieldThrower);
}

void AHDAPlayerCharacter::GiveAllAmmo() const
{
	WeaponManagerComponent->AddAmmo(EWeaponAmmoType::Gauge, 9999);
	WeaponManagerComponent->AddAmmo(EWeaponAmmoType::Bullet, 9999);
	WeaponManagerComponent->AddAmmo(EWeaponAmmoType::Energy, 9999);
	WeaponManagerComponent->AddAmmo(EWeaponAmmoType::Shield, 9999);
}
#endif
