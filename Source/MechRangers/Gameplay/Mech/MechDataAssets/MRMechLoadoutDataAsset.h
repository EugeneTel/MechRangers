// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MechRangers/Public/CombatSystem/Weapons/MRWeaponTypes.h"
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
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UMRMechModelDataAsset* MechModelAsset;

    UPROPERTY(Category=WeaponLoadout, EditDefaultsOnly, BlueprintReadOnly)
    TArray<FWeaponLoadout> WeaponLoadouts;
};

/**
 * Contains all Data for the Mech (Mech Model, Stats, Weapons etc.)
 * Must be implemented for each Mech
 */
UCLASS(BlueprintType)
class MECHRANGERS_API UMRMechLoadoutDataAsset : public UDataAsset
{
	GENERATED_BODY()

protected:

    /** Mech Loadout Structure */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FMechLoadout MechLoadout;

public:

    UFUNCTION(BlueprintCallable)
    FMechLoadout GetLoadout() const { return MechLoadout; }
};
