// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MechRangers/Gameplay/Mech/MRMech.h"

#include "MRMechControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMechControlComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	UMRMechControlComponent();

private:

	/** Controlled Mech */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	AMRMech* Mech;

	/** Cached Mech's weapon system component */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UMRWeaponSystemComponent* WeaponSystem;

public:
	/** Move Mech Forward */
	UFUNCTION(BlueprintCallable)
    void MoveForward(const float Val) const;

	/** Turn Mech At rate */
	UFUNCTION(BlueprintCallable)
    void TurnAtRate(const float Val) const;

	/** Attach a Pilot to a Mech */
	UFUNCTION(BlueprintCallable)
	void SitPilotIntoMech(AMRMech* NewMech);

	/** Set Mech combat mode */
	UFUNCTION(BlueprintCallable)
	void SetCombatMode(const bool Val);

	/** Get Mech combat mode */
	UFUNCTION(BlueprintCallable)
	bool IsCombatMode() const;

//----------------------------------------------------------------------------------------------------------------------
// Arm controls
//----------------------------------------------------------------------------------------------------------------------
public:
	
	UFUNCTION(BlueprintCallable)
	void AddArmLeftPitch(const float Val) const;

	UFUNCTION(BlueprintCallable)
	void AddArmLeftYaw(const float Val) const;

	UFUNCTION(BlueprintCallable)
	void AddArmRightPitch(const float Val) const;

	UFUNCTION(BlueprintCallable)
	void AddArmRightYaw(const float Val) const;

	UFUNCTION(BlueprintCallable)
	bool IsManipulatorLeftHeld() const;

	UFUNCTION(BlueprintCallable)
	bool IsManipulatorRightHeld() const;

//----------------------------------------------------------------------------------------------------------------------
// WeaponControls
//----------------------------------------------------------------------------------------------------------------------
public:
	/** Weapon handlers */
	void PrimaryLeftWeaponStart() const;
	void PrimaryLeftWeaponFinish() const;
	void PrimaryRightWeaponStart() const;
	void PrimaryRightWeaponFinish() const;
	void SecondaryLeftWeaponStart() const;
	void SecondaryLeftWeaponFinish() const;
	void SecondaryRightWeaponStart() const;
	void SecondaryRightWeaponFinish() const;
};
