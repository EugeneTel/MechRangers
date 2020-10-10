// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MechRangers/Gameplay/Turrets/MRTurret.h"
#include "MRTurret_Mechanic.generated.h"

class UMRDestructiblePieceComponent;

/**
 * Turret mechanic implementation
 */
UCLASS()
class MECHRANGERS_API AMRTurret_Mechanic : public AMRTurret
{
	GENERATED_BODY()

protected:

    /** Base part where will be attached a BaseRing of the turret */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UMRDestructiblePieceComponent* TurretBase;

    /** BaseRing where will be attached a Pitch of the turret */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* TurretBaseRing;

    /** Pitch where will be attached Weapons. */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* TurretPitch;
    
    virtual void FollowCombatTarget(float DeltaTime) override;

public:

    /** Constructor */
    AMRTurret_Mechanic(const class FObjectInitializer& ObjectInitializer);

    /** Turret damaged state implementation */
    virtual void Damaged() override;

    /** Turret Death implementation */
    virtual void Destroyed() override;


    
};
