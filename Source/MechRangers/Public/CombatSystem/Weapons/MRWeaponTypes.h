// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "MechRangers/Public/CombatSystem/Weapons/MRWeaponDataAsset.h"
#include "MRWeaponTypes.generated.h"

class UMRSlotTypeDataAsset;

/**
 * Weapon Group
 */
UENUM(BlueprintType)
enum class EWeaponGroup : uint8
{
    EWG_Primary UMETA(DisplayName = "Primary"),
    EWG_Secondary UMETA(DisplayName = "Secondary"),

    EWG_Invalid UMETA(DisplayName = "Invalid")
};

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
    FName SocketName;

    /** Where hardpoint should be placed */
    UPROPERTY(Category=WeaponSlot, EditDefaultsOnly, BlueprintReadWrite)
    EMechPart MechPart;

    /** Mech Slot Type Data Asset */
    UPROPERTY(Category=WeaponSlot, EditDefaultsOnly, BlueprintReadWrite)
    UMRSlotTypeDataAsset* SlotTypeAsset;

    FWeaponSlot()
    {
        HardpointSlotID = NAME_None;
        SocketName = NAME_None;
        MechPart = EMechPart::EMP_Invalid;
        SlotTypeAsset = nullptr;
    }
    
};

/**
 * Weapon Loadout
 */
USTRUCT(BlueprintType)
struct FWeaponLoadout
{
    GENERATED_BODY()

    UPROPERTY(Category=WeaponLoadout, EditDefaultsOnly, BlueprintReadWrite)
    FName HardpointSlotID;

    UPROPERTY(Category=WeaponLoadout, EditDefaultsOnly, BlueprintReadWrite)
    UMRWeaponDataAsset* WeaponAsset;

    UPROPERTY(Category=WeaponLoadout, EditDefaultsOnly, BlueprintReadWrite)
    EWeaponGroup WeaponGroup;

    FWeaponLoadout()
    {
        HardpointSlotID = NAME_None;
        WeaponAsset = nullptr;
        WeaponGroup = EWeaponGroup::EWG_Invalid;
    }
    
};

/**
 * Weapon data for spawning
 */
USTRUCT()
struct FWeaponSpawnData
{
    GENERATED_BODY()

    UPROPERTY()
    FName Name;

    UPROPERTY()
    EWeaponSize Size;

    UPROPERTY()
    EWeaponGroup Group;

    UPROPERTY()
    FGameplayTagContainer Type;

    UPROPERTY()
    FName SocketName;

    UPROPERTY()
    EMechPart MechPart;

    UPROPERTY()
    TSubclassOf<AMRWeapon> WeaponClass;

    UPROPERTY()
    FName HardpointSlotID;
};
