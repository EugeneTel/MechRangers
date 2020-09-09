// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"
#include "MechRangers/Gameplay/Weapons/MRWeaponTypes.h"

#include "MRMechHardpointDataAsset.generated.h"

class UMRSlotTypeDataAsset;

USTRUCT(BlueprintType)
struct FMechHardpoint
{
    GENERATED_BODY()

    /** List of available weapon slots */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<FWeaponSlot> WeaponSlots;

};

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRMechHardpointDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FMechHardpoint MechHardpoints;

};
