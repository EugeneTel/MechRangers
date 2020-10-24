// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MechRangers/Gameplay/Mech/MRMechTypes.h"

#include "MRMechDestructionHierarchyAsset.generated.h"

USTRUCT(BlueprintType)
struct FMechBonesToHide
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FName> BoneNames;
};

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRMechDestructionHierarchyAsset : public UDataAsset
{
	GENERATED_BODY()
public:

	/** List of bones to hide on Damaged state */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EMechPart, FMechBonesToHide> DamagedPartsToHide;

	/** List of bones to hide on Destroyed state */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EMechPart, FMechBonesToHide> DestroyedPartsToHide;
};
