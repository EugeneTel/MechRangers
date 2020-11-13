// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatSystem/MRSimpleAimComponent.h"
#include "../MRMechTypes.h"
#include "MRMechAimComponent.generated.h"

class AMRMech;
class AMRCrosshair;

/**
 * 
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MECHRANGERS_API UMRMechAimComponent : public UMRSimpleAimComponent
{
	GENERATED_BODY()

public:

	UMRMechAimComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Get Crosshair Type */
	UFUNCTION(BlueprintCallable)
    FORCEINLINE ECrosshairType GetCrosshairType() const { return CrosshairType; }

	/** Spawn Crosshair from the class */
	UFUNCTION(BlueprintCallable)
    void SpawnCrosshair();

protected:

	/** Owning mech */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	AMRMech* OwnerMech;

	/** Crosshair instance for the armed mech part */
	UPROPERTY(BlueprintReadWrite)
	AMRCrosshair* Crosshair;

	/** Crosshair Type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ECrosshairType CrosshairType;

	/** Crosshair actor class with widget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AMRCrosshair> CrosshairClass;

	void UpdateCrosshair() const;
	
};
