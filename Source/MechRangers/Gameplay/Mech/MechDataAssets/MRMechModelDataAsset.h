// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MRMechCapsuleDataAsset.h"
#include "Engine/DataAsset.h"
#include "MRMechModelDataAsset.generated.h"

class AMRMechCockpit;
class UMRMechAnimInstance;

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
	FString Socket;

	FMechCockpit()
	{
		CockpitClass = nullptr;
		Socket = FString("Cockpit");
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USkeletalMesh* SkeletalMesh;

	/** Animation blueprint for a Mech */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UMRMechAnimInstance> AnimClass;

	/** Capsule component parameters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMRMechCapsuleDataAsset* CapsuleAsset;

	/** Cockpit for VR players */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMechCockpit VRCockpit;
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
