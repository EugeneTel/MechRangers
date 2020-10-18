// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MechRangers/Gameplay/Managers/MRUnitManagerComponent.h"

#include "MRGameMode.generated.h"

class AMRMech;
class UMRMechLoadoutDataAsset;

/**
 * General Game Mode
 */
UCLASS()
class MECHRANGERS_API AMRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Setup Defaults */
	AMRGameMode();

	/** Override DefaultPawn spawn method for spawning a Mech at the same time */
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
private:

	/** Is VR Mode */
	UPROPERTY(Category=MechRangers, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	bool bVRMode;

	/** Spawned Mech */
	UPROPERTY(Category=MechRangers, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AMRMech> MechClass;

	/** Default Mech Loadout */
	UPROPERTY(Category=MechRangers, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UMRMechLoadoutDataAsset* MechLoadoutAsset;

	/** Manager responsible for spawn Units */
	UPROPERTY(Category=MechRangers, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UMRUnitManagerComponent* UnitManager;

public:

	/** Return current game mode */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsVR() const { return bVRMode; };

};
