// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"
#include "MechRangers/Public/CombatSystem/Weapons/MRWeaponTypes.h"
#include "MRMechHardpointDataAsset.generated.h"

/**
* Array of weapon slots
*/
USTRUCT(BlueprintType)
struct FMechHardpoints
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
    FMechHardpoints MechHardpoints;

};
