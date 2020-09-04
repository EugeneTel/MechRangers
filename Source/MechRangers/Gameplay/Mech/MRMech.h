// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MechComponents/MRMechLivingComponent.h"
#include "GameFramework/Character.h"
#include "MechDataAssets/MRMechLoadoutDataAsset.h"
#include "MechDataAssets/MRMechModelDataAsset.h"

#include "MRMech.generated.h"

UCLASS()
class MECHRANGERS_API AMRMech : public ACharacter
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

	/** Setup Mech from Loadout Asset */
	UFUNCTION(BlueprintCallable)
	void ConstructMech();

	/** Current Mechs Cockpit actor instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AMRMechCockpit* Cockpit;

	/** Setup Cockpit for mech from Mech Model Data */
	UFUNCTION(BlueprintCallable)
	AMRMechCockpit* SpawnCockpit(FMechCockpit CockpitData);

//----------------------------------------------------------------------------------------------------------------------
// Components
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Models the various health containing parts of a Mech, including interactions between health components. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRMechLivingComponent* LivingComponent;

//----------------------------------------------------------------------------------------------------------------------
// Configs
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Loadout asset for the Mech. Every Mech must have it! */ 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Mech)
	UMRMechLoadoutDataAsset* MechLoadoutAsset;

	/** Active Loadout structure for the Mech */
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category=Mech)
	FMechLoadout MechLoadout;

	/** Active Mech model data structure */
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category=Mech)
	FMechModelData MechModelData;

public:

	/** Set Mech loadout */
	UFUNCTION(BlueprintCallable)
	void SetLoadout(FMechLoadout NewLoadout);

//----------------------------------------------------------------------------------------------------------------------
// Input handlers
//----------------------------------------------------------------------------------------------------------------------
public:
	
	/** setup pawn specific input handlers */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Move forward/back */
	void MoveForward(float Val);

	/** Frame rate independent turn */
	void TurnAtRate(float Val);
};
