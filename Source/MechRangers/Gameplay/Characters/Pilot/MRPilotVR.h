// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VRBaseCharacter.h"
#include "MRPilotInterface.h"
#include "PilotComponents/MRMechControlComponent.h"

#include "MRPilotVR.generated.h"

//class UMRMechControlComponent;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AMRPilotVR : public AVRBaseCharacter, public IMRPilotInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AMRPilotVR();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//----------------------------------------------------------------------------------------------------------------------
// Components
//----------------------------------------------------------------------------------------------------------------------
private:

	UPROPERTY(Category=MRPilot, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UMRMechControlComponent* MechControl;

public:

	/** Get Mech Control Component */
	UFUNCTION(Category=MRPilot, BlueprintCallable)
    UMRMechControlComponent* GetMechControlComponent() const;

	//----------------------------------------------------------------------------------------------------------------------
	// Mech
	//----------------------------------------------------------------------------------------------------------------------
private:

	/** A Mech owned by the Pilot */
	UPROPERTY(Category=MRPilot, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	AMRMech* Mech;
	
	protected:

	/** Sit Pilot Into Mech. Interface method implementation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SitIntoMech(AMRMech* NewMech);

	/** Input Mech Move Forward */
	UFUNCTION(Category=MRPilot, BlueprintCallable)
    void MechMoveForward(float Val);

	/** Input Mech Turn Rate */
	UFUNCTION(Category=MRPilot, BlueprintCallable)
    void MechTurnAtRate(float Val);

	
	
};
