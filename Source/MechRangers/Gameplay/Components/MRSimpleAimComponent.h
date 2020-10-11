// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "MRSimpleAimComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRSimpleAimComponent : public USceneComponent
{
	GENERATED_BODY()

	/** Delegate Fires on Aim Hit an Actor with DamageTakerInterface */
	DECLARE_DELEGATE_OneParam(FOnDamageTakerHit, FHitResult&);

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UMRSimpleAimComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//----------------------------------------------------------------------------------------------------------------------
// Aiming
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Is Debug mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug;

	/** Inform subscribers if Hit an Actor with DamageTakerInterface */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHitInform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float TraceLength = 10000.f;

	/** Saved Trace End Point from the last Trace */
	UPROPERTY(BlueprintReadWrite)
	FVector TraceEndPoint;
public:

	/** Delegate */
	FOnDamageTakerHit OnDamageTakerHitDelegate;

	/** Get Saved Trace End Point from the last Trace */
	UFUNCTION(BlueprintCallable)
    FORCEINLINE FVector& GetTraceEndPoint() { return TraceEndPoint; }

	/** Set Trace Length for aiming */
	UFUNCTION(BlueprintCallable)
	void SetTraceLength(const float InLength) { TraceLength = InLength; }

	/** Make a line trace for aiming */
	UFUNCTION(BlueprintCallable)
    void Trace();
};
