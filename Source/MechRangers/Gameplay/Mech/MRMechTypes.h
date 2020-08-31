#pragma once

#include "MRMechTypes.generated.h"

/**
* Mech Part types. Limbs and other devices must belongs to some part type
* This parts can be damaged and destroyed.
* Also they will be shown at the status screen and represent current Mech's state
*/
UENUM(BlueprintType)
enum class EMechPart : uint8
{
    EMP_Head UMETA(DisplayName = "Head"),
    EMP_Torso UMETA(DisplayName = "Torso"),
    EMP_LeftArm UMETA(DisplayName = "LeftArm"),
    EMP_RightArm UMETA(DisplayName = "RightArm"),
    EMP_LeftLeg UMETA(DisplayName = "LeftLeg"),
    EMP_RightLeg UMETA(DisplayName = "RightLeg"),
    EMP_Invalid UMETA(DisplayName = "Invalid")
};

/**
* Mech's Part Health State
*/
UENUM(BlueprintType)
enum class EPartHealth : uint8
{
    EPH_Healthy UMETA(DisplayName = "Healthy"),
    EPH_Damaged UMETA(DisplayName = "Damaged"),
    EPH_Destroyed UMETA(DisplayName = "Destroyed"),
    EPH_Invalid UMETA(DisplayName = "Invalid")
};