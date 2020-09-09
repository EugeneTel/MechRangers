// Copyright PlatoSpace.com. All Rights Reserved.

#pragma once

#include "MechRangers/Gameplay/Mech/MRMechTypes.h"
#include "MRWeaponTypes.generated.h"

class UMRSlotTypeDataAsset;

/**
 * Weapon Slot structure
 */
USTRUCT(BlueprintType)
struct FWeaponSlot
{
    GENERATED_BODY()

    /** Unique Hardpoint Slot ID */
    UPROPERTY(Category=WeaponSlot, EditDefaultsOnly, BlueprintReadWrite)
    FName HardpointSlotID;

    /** Socket Name for weapon attachment */
    UPROPERTY(Category=WeaponSlot, EditDefaultsOnly, BlueprintReadWrite)
    FName MechSkeletonSocketID;

    /** Where hardpoint should be placed */
    UPROPERTY(Category=WeaponSlot, EditDefaultsOnly, BlueprintReadWrite)
    EMechPart MechPart;

    /** Mech Slot Type Data Asset */
    UPROPERTY(Category=WeaponSlot, EditDefaultsOnly, BlueprintReadWrite)
    UMRSlotTypeDataAsset* SlotTypeAsset;
    
};
