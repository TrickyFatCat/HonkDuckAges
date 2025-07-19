// 


#include "HDAPlayerWeaponBase.h"

#include "HDAPlayerProjectileBase.h"
#include "HDAWeaponStateController.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "HonkDuckAges/Player/Components/HDAPlayerWeaponManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AHDAPlayerWeaponBase::AHDAPlayerWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshComponent->SetRelativeLocation(FVector(30.f, 0.f, 5.f));
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	WeaponStateController = CreateDefaultSubobject<UHDAWeaponStateController>(TEXT("WeaponStateController"));

#if WITH_EDITOR
	ForwardVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardVector"));
	ForwardVector->SetupAttachment(GetRootComponent());
#endif
}

void AHDAPlayerWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		SpreadRadius = FMath::Tan(FMath::DegreesToRadians(SpreadAngleDeg)) * TraceLength;

		if (BulletType == EWeaponBulletType::Projectile)
		{
			ensureMsgf(IsValid(ProjectileClass),
			           TEXT("Invalid projectile class in %s"), *GetActorNameOrLabel());
		}

		ensureMsgf(OwningWeaponManager.IsValid(),
		           TEXT("Invalid owning weapon manager for %s"), *GetActorNameOrLabel());

		if (IsValid(MeshComponent))
		{
			ensureMsgf(MeshComponent->DoesSocketExist(SpawnSocketName),
			           TEXT("%s static mesh doesn't have socket %s"),
			           *GetActorNameOrLabel(),
			           *SpawnSocketName.ToString());
		}

		if (WeaponStateController->GetCurrentState() == EWeaponState::Disabled)
		{
			SetWeaponEnabled(false);
		}

		WeaponStateController->OnWeaponStateChanged.AddUniqueDynamic(this,
		                                                             &AHDAPlayerWeaponBase::HandleWeaponStateChanged);
	}
}

#if WITH_EDITOR
void AHDAPlayerWeaponBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	const FName RateOfFireName = GET_MEMBER_NAME_CHECKED(AHDAPlayerWeaponBase, RateOfFire);

	if (PropertyName == RateOfFireName)
	{
		if (RateOfFire <= 0.f)
		{
			RateOfFire = 1.f;
		}

		ShotDelay = 1.f / RateOfFire;
	}

	const FName ShotDelayName = GET_MEMBER_NAME_CHECKED(AHDAPlayerWeaponBase, ShotDelay);

	if (PropertyName == ShotDelayName)
	{
		if (ShotDelay <= 0.f)
		{
			ShotDelay = 1.f;
		}

		RateOfFire = 1.f / ShotDelay;
	}
}
#endif

void AHDAPlayerWeaponBase::StartShooting()
{
	if (GetCurrentState() != EWeaponState::Idle)
	{
		return;
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(ShotTimerHandle))
	{
		return;
	}

	WeaponStateController->TransitToShooting(true);
	MakeShot();
	TimerManager.SetTimer(ShotTimerHandle, this, &AHDAPlayerWeaponBase::HandleShotTimerFinished, ShotDelay, false);
	ShotTimerStarted();
}

void AHDAPlayerWeaponBase::StopShooting()
{
	if (GetCurrentState() != EWeaponState::Shooting)
	{
		return;
	}

	const bool bIsOutOfAmmo = OwningWeaponManager->GetCurrentAmmo().ReachedMinValue();
	bIsOutOfAmmo ? WeaponStateController->TransitToOutOfAmmo(true) : WeaponStateController->TransitToIdle(true);
}

void AHDAPlayerWeaponBase::ActivateWeapon()
{
	if (OwningWeaponManager->GetCurrentAmmo().ReachedMinValue())
	{
		WeaponStateController->TransitToOutOfAmmo(true);
	}
	else
	{
		WeaponStateController->TransitToIdle(true);
	}

	SetWeaponEnabled(true);
}

void AHDAPlayerWeaponBase::DeactivateWeapon()
{
	WeaponStateController->TransitToDisabled(true);
}

void AHDAPlayerWeaponBase::SetOwningWeaponManager(UHDAPlayerWeaponManager* NewManager)
{
	if (!IsValid(NewManager) || OwningWeaponManager.IsValid())
	{
		return;
	}

	OwningWeaponManager = NewManager;
	OwningWeaponManager->OnAmmoIncreased.AddUniqueDynamic(this, &AHDAPlayerWeaponBase::HandleAmmoIncreased);
}

float AHDAPlayerWeaponBase::GetNormalizedRemainingShotTime() const
{
	const FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (ShotDelay <= 0.f || !TimerManager.IsTimerActive(ShotTimerHandle))
	{
		return -1.f;
	}

	return TimerManager.GetTimerRemaining(ShotTimerHandle) / ShotDelay;
}

float AHDAPlayerWeaponBase::GetNormalizedElapsedShotTime() const
{
	const FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	if (ShotDelay <= 0.f || !TimerManager.IsTimerActive(ShotTimerHandle))
	{
		return -1.f;
	}

	return TimerManager.GetTimerElapsed(ShotTimerHandle) / ShotDelay;
}

void AHDAPlayerWeaponBase::CalculateBulletDisplacement(FVector2D& Displacement) const
{
	if (SpreadAngleDeg <= 0.f)
	{
		Displacement = FVector2D::ZeroVector;
		return;
	}

	float DistributionCoefficient = 1.f;

	if (IsValid(SpreadDistribution))
	{
		DistributionCoefficient = SpreadDistribution->GetFloatValue(FMath::RandRange(0.0, 1.0));
	}

	const float AngleRad = FMath::RandRange(0.f, TWO_PI);
	const float Distance = FMath::RandRange(0.f, SpreadRadius * DistributionCoefficient);
	Displacement = FVector2D(FMath::Cos(AngleRad) * Distance, FMath::Sin(AngleRad) * Distance);
}

void AHDAPlayerWeaponBase::MakeShot()
{
	if (BulletsPerShot <= 0 || GetCurrentState() != EWeaponState::Shooting)
	{
		return;
	}

	const TWeakObjectPtr<UCameraComponent> CameraComponent = OwningWeaponManager->GetCameraComponent();

	if (!CameraComponent.IsValid())
	{
		return;
	}

	FMinimalViewInfo ViewInfo;
	CameraComponent->GetCameraView(0.f, ViewInfo);

	const FVector StartLocation = ViewInfo.Location;
	const FVector CameraFwdVec = UKismetMathLibrary::GetForwardVector(ViewInfo.Rotation);
	const FVector CameraRightVec = UKismetMathLibrary::GetRightVector(ViewInfo.Rotation);
	const FVector CameraUpVec = UKismetMathLibrary::GetUpVector(ViewInfo.Rotation);

	const TArray<AActor*> ActorsToIgnore{this, GetOwner()};
	FVector2D Displacement = FVector2D::ZeroVector;
	FHitResult HitResult;
	TArray<FHitResult> HitResults;

	for (int32 i = 0; i < BulletsPerShot; ++i)
	{
		CalculateBulletDisplacement(Displacement);
		FVector EndLocation = StartLocation + CameraFwdVec * TraceLength;
		EndLocation += CameraRightVec * Displacement.X;
		EndLocation += CameraUpVec * Displacement.Y;

		UKismetSystemLibrary::LineTraceSingle(this,
		                                      StartLocation,
		                                      EndLocation,
		                                      TraceTypeQuery1,
		                                      false,
		                                      ActorsToIgnore,
		                                      EDrawDebugTrace::None,
		                                      HitResult,
		                                      true,
		                                      FLinearColor::Red,
		                                      FLinearColor::Green,
		                                      0.25);
		HitResults.Add(HitResult);

		switch (BulletType)
		{
		case EWeaponBulletType::Trace:
			if (HitResult.bBlockingHit && IsValid(HitResult.GetActor()))
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(),
				                              Damage,
				                              GetInstigatorController(),
				                              GetOwner(),
				                              nullptr);
			}
			break;

		case EWeaponBulletType::Projectile:

			SpawnProjectile(HitResult);
			break;
		}
	}

	OnPlayerWeaponShot.Broadcast(this);
	WeaponShot(HitResults);
}

AHDAPlayerProjectileBase* AHDAPlayerWeaponBase::SpawnProjectile(const FVector& Direction)
{
	if (!IsValid(ProjectileClass))
	{
		return nullptr;
	}

	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(GetActorLocation());

	if (IsValid(MeshComponent) && MeshComponent->DoesSocketExist(SpawnSocketName))
	{
		SpawnTransform.SetLocation(MeshComponent->GetSocketLocation(SpawnSocketName));
	}

	AHDAPlayerProjectileBase* NewProjectile = GetWorld()->SpawnActorDeferred<AHDAPlayerProjectileBase>(ProjectileClass,
		SpawnTransform,
		this,
		Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn,
		ESpawnActorScaleMethod::SelectDefaultAtRuntime);
	NewProjectile->InitProjectile(HitResult, Damage);
	NewProjectile->FinishSpawning(SpawnTransform);
	return NewProjectile;
}

void AHDAPlayerWeaponBase::HandleAmmoIncreased(UHDAPlayerWeaponManager* Component,
                                               EWeaponAmmoType AmmoType,
                                               const FTrickyPropertyInt& Ammo,
                                               int32 DeltaValue)
{
	if (!Ammo.ReachedMinValue() && GetCurrentState() == EWeaponState::OutOfAmmo)
	{
		WeaponStateController->TransitToIdle(true);
	}
}

void AHDAPlayerWeaponBase::SetWeaponEnabled(const bool bIsEnabled)
{
	MeshComponent->SetHiddenInGame(!bIsEnabled);
	SetActorTickEnabled(bIsEnabled);
}

void AHDAPlayerWeaponBase::HandleWeaponStateChanged(UHDAWeaponStateController* Component,
                                                    EWeaponState NewState,
                                                    bool bTransitImmediately)
{
	switch (NewState)
	{
	case EWeaponState::Idle:
		break;
	case EWeaponState::OutOfAmmo:
		break;
	case EWeaponState::Disabled:
		SetWeaponEnabled(false);
		break;
	}
}

void AHDAPlayerWeaponBase::HandleShotTimerFinished()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(ShotTimerHandle);
	ShotTimerFinished();

	switch (WeaponMode)
	{
	case EWeaponMode::SemiAuto:
		StopShooting();
		break;

	case EWeaponMode::FullAuto:
		MakeShot();

		if (GetCurrentState() == EWeaponState::Shooting)
		{
			TimerManager.SetTimer(ShotTimerHandle,
			                      this,
			                      &AHDAPlayerWeaponBase::HandleShotTimerFinished,
			                      ShotDelay,
			                      false);
			ShotTimerStarted();
		}
		break;
	}
}
