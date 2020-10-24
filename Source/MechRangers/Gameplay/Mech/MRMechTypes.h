// Copyright PlatoSpace.com. All Rights Reserved.

#pragma once
#include "MRMechTypes.generated.h"

class AMRMechAim;
class USoundBase;

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
    EMP_LeftArm UMETA(DisplayName = "Left Arm"),
    EMP_RightArm UMETA(DisplayName = "Right Arm"),
    EMP_LeftLeg UMETA(DisplayName = "Left Leg"),
    EMP_RightLeg UMETA(DisplayName = "Righ tLeg"),
    EMP_Invalid UMETA(DisplayName = "Invalid")
};

/**
* Mech's Part Health State
*/
UENUM(BlueprintType)
enum class EPartHealthState : uint8
{
    PHS_Healthy UMETA(DisplayName = "Healthy"),
    PHS_Damaged UMETA(DisplayName = "Damaged"),
    PHS_Destroyed UMETA(DisplayName = "Destroyed"),
    PHS_Invalid UMETA(DisplayName = "Invalid")
};

/**
 * Weapon's size list
 */
UENUM(BlueprintType)
enum class EWeaponSize : uint8
{
    EWS_Small UMETA(DisplayName = "Small"),
    EWS_Medium UMETA(DisplayName = "Medium"),
    EWS_Large UMETA(DisplayName = "Large"),
    EWS_Invalid UMETA(DisplayName = "Invalid"),
};

/** Weapon Crosshair Type */
UENUM(BlueprintType)
enum class ECrosshairType : uint8
{
    ECT_None UMETA(DisplayName = "None"),
    ECT_Basic UMETA(DisplayName = "Basic")
};

/**
 * Mech's weapon aim configuration
 */
USTRUCT(BlueprintType)
struct FMechAim
{
    GENERATED_BODY()

    /** Cockpit attachment socket */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
    FName Socket;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TSubclassOf<AMRMechAim> MechAimClass;
};


USTRUCT(BlueprintType)
struct FMechParticleSpawnData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName BoneName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UParticleSystem* ParticleSystem = nullptr;
};

/** Mech sound data */
USTRUCT(BlueprintType)
struct FMechSoundSpawnData
{
    GENERATED_BODY()
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName BoneName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    USoundBase* Audio = nullptr;

};