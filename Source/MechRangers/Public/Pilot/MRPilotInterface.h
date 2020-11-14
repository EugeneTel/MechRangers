// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MechRangers/Gameplay/Mech/MRMech.h"
#include "UObject/Interface.h"
#include "MRPilotInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMRPilotInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MECHRANGERS_API IMRPilotInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/** Attach Pilot to Mech */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SitIntoMech(AMRMech* NewMech);
};
