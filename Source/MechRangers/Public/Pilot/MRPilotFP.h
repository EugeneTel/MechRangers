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

	/**  MoveForward or Left Arm Yaw (Y-axis) control for Gamepad controls */
	void Move_ArmLeftYaw(float Val);

	/**  TurnRate or Left Arm Pitch (X-axis) control for Gamepad controls */
	void Turn_ArmLeftPitch(float Val);
	
	/**  LookRight or Right Arm Pitch (X-axis) control for Gamepad controls */
	void LookRight_ArmRightPitch(float Val);

	/**  LookUp or Right Arm Yaw (Y-axis) control for Gamepad controls */
	void LookUp_ArmRightYaw(float Val);

	/**  Change combat mode */
	void CombatModePressed();

	/** Action handlers */
	void PrimaryLeftActionPressed();
	void PrimaryLeftActionReleased();
	void PrimaryRightActionPressed();
	void PrimaryRightActionReleased();
	void SecondaryLeftActionPressed();
	void SecondaryLeftActionReleased();
	void SecondaryRightActionPressed();
	void SecondaryRightActionReleased();

//----------------------------------------------------------------------------------------------------------------------
// Control
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Is Combat mode or Movement mode. */
	UPROPERTY(Category=MRPilot, VisibleInstanceOnly, BlueprintReadWrite)
	bool bIsCombatMode;
	
	/** Pilot LookUp */
	void LookUp(float Val);

	/** Pilot LookRight */
	void LookRight(float Val);
};
