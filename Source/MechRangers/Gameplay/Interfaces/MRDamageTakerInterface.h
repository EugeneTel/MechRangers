// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MechRangers/MechRangersTypes.h"
#include "UObject/Interface.h"
#include "MRDamageTakerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMRDamageTakerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MECHRANGERS_API IMRDamageTakerInterface
{
	GENERATED_BODY()

public:

	/** Delegate when an actor was destroyed */
	DECLARE_EVENT_OneParam(IMRDamageTakerInterface, FOnDeath, AActor*);
	virtual FOnDeath& OnDeath() = 0;

	/** Get damage taker Team Id. From the same team will not attack each other */
	virtual EGameplayTeam GetGameplayTeam() const = 0;

	/** A chance that attacker will attack a current object (if has other agro object). From 0.0 (will not attack) to 1.0 (attack) */
    virtual float GetAgroChance() const = 0;

	/** Is the object alive */
	virtual bool Alive() const = 0;
};
