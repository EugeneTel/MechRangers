// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseLimb.h"
#include "BaseMech.generated.h"

class UCapsuleComponent;
class USphereComponent;
class APilotCharacter;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API ABaseMech : public APawn
{
	GENERATED_BODY()

protected:
	/** Root component for the mech */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* RootReference;

	/** Mech's Cabin */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* Cabin;
	
public:

	/** Constructor */
	ABaseMech();

	void BeginPlay() override;


//----------------------------------------------------------------------------------------------------------------------
// Limbs
//----------------------------------------------------------------------------------------------------------------------

protected:
	
	/** Available Limbs (hands, weapons, etc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<ELimbType, ABaseLimb*> Limbs;

public:
	
	/** Get Available limb by type */
	UFUNCTION(BlueprintCallable)
    FORCEINLINE ABaseLimb* GetLimb(const ELimbType LimbType) const { return Limbs.FindRef(LimbType); };

	/** Get all registered limbs */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TMap<ELimbType, ABaseLimb*>& GetLimbs() { return Limbs; }

	/** Control selected Limb (rotate, move etc.) */
	UFUNCTION(BlueprintCallable)
	virtual void LimbControl(float AxisX, float AxisY, ELimbType LimbType);

//----------------------------------------------------------------------------------------------------------------------
// Pilot
//----------------------------------------------------------------------------------------------------------------------

protected:
	/** Owned By Pilot Character */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Pilot")
	APilotCharacter* OwnedPilot;
	
	/** Where a pilot should be attached */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pilot|Components")
	USceneComponent* PilotAttachmentPoint;

	/** OwnedPilot head zone visualization */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pilot|Components")
	USphereComponent* HeadZoneVisualizer;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Pilot")
	bool bVRMode;

	/** OwnedPilot Teking Control of the current Mech */
	UFUNCTION(BlueprintCallable)
	void PilotTakingControl(APilotCharacter* PilotRef, bool bTaking, USceneComponent* AttachPoint, FTransform RelativeTransformToSeat, FTransform InitialCameraTransform);

public:

	/** Get Owned Pilot Character */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE APilotCharacter* GetOwnedPilot() const { return OwnedPilot; }
	
	/** Sit pilot down to the Mech and takes control of it */
	UFUNCTION(BlueprintCallable)
    void PilotSitDown(APilotCharacter* PilotRef);

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------

public:
	/** Starts weapon fire from defined limb and socket */
	UFUNCTION(BlueprintCallable)
	void LimbStartFire(ELimbType LimbType, ELimbSocket Socket);

	/** Stops weapon fire from defined limb and socket */
	UFUNCTION(BlueprintCallable)
    void LimbStopFire(ELimbType LimbType, ELimbSocket Socket);
	
};
