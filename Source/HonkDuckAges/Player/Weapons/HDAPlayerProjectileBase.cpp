// 


#include "HDAPlayerProjectileBase.h"

#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AHDAPlayerProjectileBase::AHDAPlayerProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetCollisionProfileName(TEXT("PlayerProjectile"));

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;

#if WITH_EDITOR
	ForwardVector = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ForwardVector"));
#endif
}

void AHDAPlayerProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();

	if (IsValid(World) && World->IsGameWorld())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
		CollisionComponent->IgnoreActorWhenMoving(this, true);
		CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);

		CollisionComponent->OnComponentHit.AddUniqueDynamic(this, &AHDAPlayerProjectileBase::HandleHit);
	}
}

void AHDAPlayerProjectileBase::InitProjectile(const FHitResult& HitResult, const int32 NewDamage)
{
	if (NewDamage > 0)
	{
		Damage = NewDamage;
	}
	const FVector TargetPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : HitResult.TraceEnd;
	const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), TargetPoint);
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
	ProjectileMovementComponent->UpdateComponentVelocity();
	ProjectileMovementComponent->Activate();
}

void AHDAPlayerProjectileBase::HandleHit(UPrimitiveComponent* HitComponent,
                                         AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp,
                                         FVector NormalImpulse,
                                         const FHitResult& Hit)
{
	if (IsValid(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
	}

	if (bDestroyOnHit)
	{
		Destroy();
	}
}
