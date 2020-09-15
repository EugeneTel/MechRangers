// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MechRangers/Gameplay/Mech/MRMechTypes.h"
#include "MRWeaponDataAsset.generated.h"

class AMRWeapon;
class UMRSlotTypeDataAsset;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category=General, EditDefaultsOnly, BlueprintReadWrite)
	FName Name;

	UPROPERTY(Category=General, EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AMRWeapon> WeaponClass;

	UPROPERTY(Category=EquipRules, EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTagContainer WeaponType;

	UPROPERTY(Category=EquipRules, EditDefaultsOnly, BlueprintReadWrite)
	UMRSlotTypeDataAsset* WeaponSlotTypeAsset;

	UPROPERTY(Category=EquipRules, EditDefaultsOnly, BlueprintReadWrite)
	EWeaponSize WeaponSize;
	
};
