// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Mech/MRMechTypes.h"
#include "MRSlotTypeDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRSlotTypeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(Category=SlotType, EditDefaultsOnly, BlueprintReadWrite)
	FName SlotName;

	/** Allowed weapon size for this slot */
	UPROPERTY(Category=SlotType, EditDefaultsOnly, BlueprintReadWrite)
	EWeaponSize WeaponSize;

	/** Allowed weapon types for this slot */
	UPROPERTY(Category=SlotType, EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTagContainer WeaponTypesTagContainer;
};
