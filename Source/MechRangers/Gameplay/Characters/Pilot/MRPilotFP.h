// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MRPilotInterface.h"
#include "GameFramework/Character.h"
#include "MechRangers/Gameplay/Mech/MRMech.h"

#include "MRPilotFP.generated.h"

class UMRMechControlComponent;
class UCameraComponent;

UCLASS()
class MECHRANGERS_API AMRPilotFP : public ACharacter, public IMRPilotInterface
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	AMRPilotFP();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//----------------------------------------------------------------------------------------------------------------------
// Components
//----------------------------------------------------------------------------------------------------------------------
private:

	UPROPERTY(Category=MRPilot, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UMRMechControlComponent* MechControl;

	UPROPERTY(Category=MRPilot, VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

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

	/** Sit Pilot Into Mech. Inteface method implementation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SitIntoMech(AMRMech* NewMech);


//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Input Mech Move Forward */
	void MechMoveForward(float Val);

	/** Input Mech Turn Rate */
    void MechTurnAtRate(float Val);

	/**  MoveForward or Limb left Y-axis control for Gamepad controls */
	void Move_LimbLeftY(float Val);

	/**  TurnRate or Limb left X-axis control for Gamepad controls */
	void Turn_LimbLeftX(float Val);
	
	/**  LookRight or Limb right X-axis control for Gamepad controls */
	void LookRight_LimbRightX(float Val);

	/**  LookUp or Limb right Y-axis control for Gamepad controls */
	void LookUp_LimbRightY(float Val);

	/**  Change movement mode */
	void ChangeMovementModePressed();

//----------------------------------------------------------------------------------------------------------------------
// Control
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Movement control mode. */
	bool bIsMovementMode;
	
	/** Pilot LookUp */
	void LookUp(float Val);

	/** Pilot LookRight */
	void LookRight(float Val);
};
