// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MRMechCapsuleDataAsset.h"
#include "Engine/DataAsset.h"
#include "MechRangers/Gameplay/Mech/MRMechTypes.h"
#include "MRMechModelDataAsset.generated.h"

class AMRMechCockpit;
class UMRMechAnimInstance;
class UMRMechHardpointDataAsset;

/**
 * Structure for Mech Cockpit
 */
USTRUCT(BlueprintType)
struct FMechCockpit
{
	GENERATED_BODY()

	/** Cockpit blueprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMRMechCockpit> CockpitClass;

	/** Cockpit attachment socket */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Socket;

	FMechCockpit()
	{
		CockpitClass = nullptr;
		Socket = FName("Cockpit");
	}
	
};

/**
* Structure for the mech model
*/
USTRUCT(BlueprintType)
struct FMechModelData
{
	GENERATED_BODY()

	/** Skeletal mesh of a Mech */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* SkeletalMesh;

	/** Animation blueprint for a Mech */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMRMechAnimInstance> AnimClass;

	/** Capsule component parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMRMechCapsuleDataAsset* CapsuleAsset;

	/** Hardpoint asset which contains all weapon slots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMRMechHardpointDataAsset* WeaponHardpointAsset;

	/** Mech Aim system configuration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMechAim MechAimConfig;

	/** Cockpit for VR players */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMechCockpit VRCockpit;
	
	/** Cockpit for First Person players */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMechCockpit FPCockpit;
};

/**
 * Mech Model contains all technical information for mech building
 */
UCLASS(BlueprintType)
class MECHRANGERS_API UMRMechModelDataAsset : public UDataAsset
{
	GENERATED_BODY()

protected:

	/** Structure represents a Mech */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMechModelData MechModelData;

public:

	UFUNCTION(BlueprintCallable)
	FMechModelData GetModelData() const { return MechModelData; }
	
};
