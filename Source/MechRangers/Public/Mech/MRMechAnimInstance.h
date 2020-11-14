// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MRMech.h"
#include "Animation/AnimInstance.h"
#include "MRMechAnimInstance.generated.h"

/**
 * General animation instance for Mech Animation Blueprints
 */
UCLASS(Abstract)
class MECHRANGERS_API UMRMechAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
    /** Save last actor rotation for TurnSpeed calculation */
    FRotator LastKnownRot;
    
protected:

    /** Current Mech */
    UPROPERTY(BlueprintReadWrite)
    AMRMech* MechOwner;
    
    /** Magnitude of velocity */
    UPROPERTY(Category=MRMechAnim, VisibleInstanceOnly, BlueprintReadWrite)
    float Speed;

    /** Speed at which the Mech turning in Degrees/Sec. Sign indicates turn directions. */
    UPROPERTY(Category=MRMechAnim, VisibleInstanceOnly, BlueprintReadWrite)
    float TurnSpeed;

    /** Is Mech combat mode enabled */
    UPROPERTY(Category=MRMechAnim, VisibleInstanceOnly, BlueprintReadWrite)
    bool bIsCombatMode;

    /** Right Arm Aim Rotator */
    UPROPERTY(Category=MRMechAnim, VisibleInstanceOnly, BlueprintReadWrite)
    FRotator ArmRightAimRotator;

    /** Left Arm Aim Rotator */
    UPROPERTY(Category=MRMechAnim, VisibleInstanceOnly, BlueprintReadWrite)
    FRotator ArmLeftAimRotator;

public:

    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
