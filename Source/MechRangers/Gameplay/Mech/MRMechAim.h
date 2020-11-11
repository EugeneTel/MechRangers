// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MRMechTypes.h"
#include "MRMechAim.generated.h"

class AMRCrosshair;
class AMRMech;

UCLASS()
class MECHRANGERS_API AMRMechAim : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AMRMechAim();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//----------------------------------------------------------------------------------------------------------------------
// Aiming
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Is Debug mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug;

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

	/** Saved Trace End Point from the last Trace */
	UPROPERTY(BlueprintReadWrite)
	FVector TraceEndPoint;
	
public:

	/** Get Saved Trace End Point from the last Trace */
	UFUNCTION(BlueprintCallable)
    FORCEINLINE FVector& GetTraceEndPoint() { return TraceEndPoint; }

	/** Get Crosshair Type */
	UFUNCTION(BlueprintCallable)
    FORCEINLINE ECrosshairType GetCrosshairType() const { return CrosshairType; }
	
	/** Make a line trace for aiming */
	UFUNCTION(BlueprintCallable)
    void Trace();

	/** Spawn Crosshair from the class */
	UFUNCTION(BlueprintCallable)
    void SpawnCrosshair();

};
