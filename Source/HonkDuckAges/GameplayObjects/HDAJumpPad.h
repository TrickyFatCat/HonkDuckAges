// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDAJumpPad.generated.h"

class UBoxComponent;
class UArrowComponent;
class UTextRenderComponent;

UCLASS()
class HONKDUCKAGES_API AHDAJumpPad : public AActor
{
	GENERATED_BODY()

public:
	AHDAJumpPad();

protected:
	virtual void PostInitializeComponents() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> ActivationTriggerComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UMeshComponent> MeshComponent = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category="JumpPad",
		meta=(ClampMin=100, UIMin=100, Delta=100, ForceUnits="Centimeters"))
	float JumpHeight = 100.0f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> UpVector = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	TObjectPtr<UTextRenderComponent> TextDebug = nullptr;
#endif

private:
	UFUNCTION()
	void HandleTriggerEntered(UPrimitiveComponent* OverlappedComponent,
	                          AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex,
	                          bool bFromSweep,
	                          const FHitResult& SweepResult);
#if WITH_EDITOR
	void UpdateDebugInfo() const;
#endif
};
