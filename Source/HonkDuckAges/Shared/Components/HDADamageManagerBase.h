// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDADamageManagerBase.generated.h"

class UHDAHealthComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvulnerabilityChangedDynamicSignature,
                                            UHDADamageManagerBase*, Component);

UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent))
class HONKDUCKAGES_API UHDADamageManagerBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UHDADamageManagerBase();

protected:
	virtual void InitializeComponent() override;

public:
	UPROPERTY(BlueprintAssignable, Category="DamageManager")
	FOnInvulnerabilityChangedDynamicSignature OnInvulnerabilityEnabled;
	
	UPROPERTY(BlueprintAssignable, Category="DamageManager")
	FOnInvulnerabilityChangedDynamicSignature OnInvulnerabilityDisabled;
	
	UFUNCTION(BlueprintGetter, Category="DamageManager")
	bool GetIsInvulnerable() const { return bIsInvulnerable; };

	UFUNCTION(BlueprintSetter, Category="DamageManager")
	void SetIsInvulnerable(const bool Value);

protected:
	UPROPERTY()
	TObjectPtr<UHDAHealthComponent> HealthComponent = nullptr;

private:
	UPROPERTY(VisibleInstanceOnly,
		BlueprintGetter=GetIsInvulnerable,
		BlueprintSetter=SetIsInvulnerable,
		Category="DamageManager")
	bool bIsInvulnerable = false;

	UFUNCTION();
	virtual void HandleDamageTaken(AActor* DamagedActor,
	                               float Damage,
	                               const UDamageType* DamageType,
	                               AController* InstigatedBy,
	                               AActor* DamageCauser);
};
