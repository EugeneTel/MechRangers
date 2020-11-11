// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MechComponents/MRMechLivingComponent.h"
#include "GameFramework/Character.h"
#include "MechComponents/MRMechHitReactionComponent.h"
#include "MechComponents/MRWeaponSystemComponent.h"
#include "MechDataAssets/MRMechDestructionHierarchyAsset.h"
#include "MechDataAssets/MRMechLoadoutDataAsset.h"
#include "MechDataAssets/MRMechModelDataAsset.h"
#include "MechRangers/Gameplay/Interfaces/MRDamageTakerInterface.h"

#include "MRMech.generated.h"

UCLASS()
class MECHRANGERS_API AMRMech : public ACharacter, public IMRDamageTakerInterface
{
	GENERATED_BODY()

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Sets default values for this character's properties
	AMRMech(const class FObjectInitializer& ObjectInitializer);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** a Mech takes a Damage */
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


protected:
	virtual float InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual float InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	/** Setup Mech from Loadout Asset */
	UFUNCTION(BlueprintCallable)
	void ConstructMech();

	/** Current Mechs Cockpit actor instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AMRMechCockpit* Cockpit;

	/** Setup Cockpit for mech from Mech Model Data */
	UFUNCTION(BlueprintCallable)
	virtual AMRMechCockpit* SpawnCockpit(FMechCockpit& CockpitData);

	UFUNCTION(BlueprintCallable)
	bool HasPilot() const;

//----------------------------------------------------------------------------------------------------------------------
// Components
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Models the various health containing parts of a Mech, including interactions between health components. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRMechLivingComponent* LivingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRWeaponSystemComponent* WeaponSystemComponent;

	/** Component responsible for all hit reactions */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRMechHitReactionComponent* HitReactionComponent;


//----------------------------------------------------------------------------------------------------------------------
// Delegates
//----------------------------------------------------------------------------------------------------------------------
public:
	/** On Death Delegate */
	FOnDeath OnDeathEvent;
	
	virtual FOnDeath& OnDeath() override;

//----------------------------------------------------------------------------------------------------------------------
// Configs
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Gameplay team (on which side a mech) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameplayTeam GameplayTeam;

	/**  A chance that attacker will attack a current object (if has other agro object). From 0.0 (will not attack) to 1.0 (attack) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AgroChance;

	/** Loadout asset for the Mech. Every Mech must have it! */ 
	UPROPERTY(Category=MechData, BlueprintReadOnly, EditAnywhere)
	UMRMechLoadoutDataAsset* MechLoadoutAsset;

	/** Active Loadout structure for the Mech */
	UPROPERTY(Category=MechData, BlueprintReadOnly, VisibleInstanceOnly)
	FMechLoadout MechLoadout;

	/** Active Mech model data structure */
	UPROPERTY(Category=MechData, BlueprintReadOnly, VisibleInstanceOnly)
	FMechModelData MechModelData;
	
	/** Is Combat mode or Movement mode. */
	UPROPERTY(Category=MRMech, VisibleInstanceOnly, BlueprintReadWrite)
	bool bIsCombatMode;

	UPROPERTY(Category=MRMech, EditAnywhere, BlueprintReadWrite)
	EMechControlType ControlType;

	/** Set Mech Control Type */
	virtual void SetupControlType();
	
public:

	/** Set Mech loadout */
	UFUNCTION(BlueprintCallable)
	void SetLoadout(const FMechLoadout& NewLoadout);
	
	/** Checks is Mech in combat mode */
	UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsCombatMode() const { return bIsCombatMode; }

	/** Set Mech Combat Mode state */
	UFUNCTION(BlueprintCallable)
    void SetCombatMode(bool const Val);

	/** Get Weapon System component */
	UFUNCTION(BlueprintCallable)
	UMRWeaponSystemComponent* GetWeaponSystem() const;

	virtual EGameplayTeam GetGameplayTeam() const override;

	virtual float GetAgroChance() const override;

	virtual bool Alive() const override;

//----------------------------------------------------------------------------------------------------------------------
// Movement controls
//----------------------------------------------------------------------------------------------------------------------
public:
	/** Move forward/back */
	void MoveForward(float Val);

	/** Frame rate independent turn */
	void TurnAtRate(float Val);

//----------------------------------------------------------------------------------------------------------------------
// Arm controls
//----------------------------------------------------------------------------------------------------------------------
protected:
	UPROPERTY(BlueprintReadWrite)
	FRotator ArmLeftRotator;

	UPROPERTY(BlueprintReadWrite)
	FRotator ArmRightRotator;

	/** Is Left VR Manipulator Held by player */ 
	UPROPERTY(BlueprintReadWrite)
	bool bManipulatorLeftHeld;
	
	/** Is Right VR Manipulator Held by player */ 
	UPROPERTY(BlueprintReadWrite)
	bool bManipulatorRightHeld;

	/** Add rotation to arms with limitation */
	void AddArmRotator(FRotator& ArmRotator, const FRotator& AddRot);
	
public:

	FORCEINLINE FRotator GetArmLeftRotator() const { return ArmLeftRotator; }
	FORCEINLINE FRotator GetArmRightRotator() const { return ArmRightRotator; }

	FORCEINLINE bool IsManipulatorLeftHeld() const { return bManipulatorLeftHeld; }
	FORCEINLINE bool IsManipulatorRightHeld() const { return bManipulatorRightHeld; }

	/** Set VR Manipulator Held state for Mech Part */
	UFUNCTION(BlueprintCallable)
	void SetManipulatorHeld(EMechPart MechPart, bool IsHeld);

	/** Add Left Arm Rotation */
	void AddArmLeftRotator(const FRotator& Rot);

	/** Add Right Arm Rotation */
	void AddArmRightRotator(const FRotator& Rot);

	/** Set Rotator for mech part */
	UFUNCTION(BlueprintCallable)
	void SetMechPartRotator(const EMechPart MechPart, const FRotator& Rot);

//----------------------------------------------------------------------------------------------------------------------
// Mech
//----------------------------------------------------------------------------------------------------------------------
protected:
	/** Hide Mech part by bone */
	void DestroyPart(EMechPart MechPart, EHealthState HealthState);

	/** Hide Mech part by bone */
	void ReplacePart(EMechPart MechPart, EHealthState HealthState);

	/** Get Mech replacement part for provided Mech Part and Health State */
	bool GetReplacementPart(EMechPart MechPart, EHealthState HealthState, FMechPartUpdateData& OutMechPartReplacements);

	/** Get Mech Bones to Hide for provided Mech Part and Health State */
	bool GetBonesToHide(EMechPart MechPart, EHealthState HealthState, FMechBonesToHide& OutMechBonesToHide);

	/** Get List of mech part replacements (list of bones with meshes) */
	bool GetMechPartUpdateData(EMechPart MechPart, EHealthState HealthState, FMechPartUpdateData& OutMechPartUpdateData);

	void SpawnParticle(FMechParticleSpawnData& ParticleData);

	void SpawnSound(FMechSoundSpawnData& SoundData);
	
public:

	UFUNCTION(BlueprintCallable)
	void DestroyPart(EMechPart MechPart);
	
	UFUNCTION(BlueprintCallable)
    void DamagePart(EMechPart MechPart);

	/** Mech Death Implementation */
	UFUNCTION(BlueprintCallable)
    void Death();
	
};