// Copyright PlatoSpace.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MRMechLoadoutDataAsset.generated.h"

class UMRMechModelDataAsset;

/**
 * Contains all Data for the Mech (Mech Model, Stats, Weapons etc.)
 */
USTRUCT(BlueprintType)
struct FMechLoadout
{
    GENERATED_BODY()

    /** Mech Model Data Asset with technical information for a Mech */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UMRMechModelDataAsset* MechModelAsset;
};

/**
 * Contains all Data for the Mech (Mech Model, Stats, Weapons etc.)
 * Must be implemented for each Mech
 */
UCLASS(BlueprintType)
class MECHRANGERS_API UMRMechLoadoutDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

    /** Mech Loadout Structure */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMechLoadout MechLoadout;
    
};
