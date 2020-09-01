// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MechRangersGameMode.generated.h"

class AMRMech;
class UMRMechLoadoutDataAsset;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AMechRangersGameMode : public AGameMode
{
	GENERATED_BODY()

public:

    AMechRangersGameMode();

    /** Is VR Mode */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=MechRangers)
    bool bVRMode;

    /** Spawned Mech */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=MechRangers)
    TSubclassOf<AMRMech> MechClass;

    /** Default Mech Loadout */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=MechRangers)
    UMRMechLoadoutDataAsset* MechLoadoutAsset;
};
