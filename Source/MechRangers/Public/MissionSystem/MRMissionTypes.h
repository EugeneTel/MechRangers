// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "MRMissionTypes.generated.h"

UENUM(BlueprintType)
enum class EMissionType : uint8
{
    All,
    Main,
    Side
};

UENUM(BlueprintType)
enum class EMissionStatus : uint8
{
    Pending,
    InProgress,
    Completed,
    Failed
};

UENUM(BlueprintType)
enum class EMissionTargetType : uint8
{
    Enemy,
    Collectable,
    Overlap,
    Interactable
};

UENUM(BlueprintType)
enum class EMissionTargetAction : uint8
{
    Destroy,
    Protect,
    Interact,
    Find
};

UENUM(BlueprintType)
enum class EMissionObjectiveType : uint8
{
    SingleTarget,
    MultipleTargets
};

USTRUCT(BlueprintType)
struct FMissionTarget
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    FName TargetID;

    /** Should it check the ID of the actors? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    bool bTargetIDMatters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    TSubclassOf<AActor> ActorClass;

    /** Should it check the Class of the actors? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    bool bActorClassMatters;

    /** Number of targets should be processed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    int32 Amount;

    /** List of actions for Success */
    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    TArray<EMissionTargetAction> SuccessOnActions;

    /** List of actions for Failed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    TArray<EMissionTargetAction> FailedOnActions;

    /** Is target mandatory for the mission. Mission fails if any of Mandatory target will fails */
    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    bool bMandatory;

    /** Number of Currently processed targets */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 CurrentAmount;

    /** Current target status */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    EMissionStatus Status;

    FMissionTarget(): bTargetIDMatters(false), bActorClassMatters(false), Amount(0), bMandatory(false), CurrentAmount(0), Status(EMissionStatus::Pending)
    {
    }

    FORCEINLINE bool operator==(const FMissionTarget& Other) const
    {
        return TargetID == Other.TargetID &&
            bTargetIDMatters == Other.bTargetIDMatters &&
            ActorClass == Other.ActorClass &&
            bActorClassMatters == Other.bActorClassMatters &&
            Amount == Other.Amount;
    }
};

USTRUCT(BlueprintType)
struct FMissionTargetData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* ActorReference;

    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    FVector Location;

    FMissionTargetData(): ActorReference(nullptr)
    {
    }
};

USTRUCT(BlueprintType)
struct FMissionTargetInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    FName TargetID;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
    TSubclassOf<AActor> ActorClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    EMissionTargetType Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite);
    FMissionTargetData TargetData;

    FMissionTargetInfo() : Type(EMissionTargetType::Collectable), TargetData()
    {
    }
};

USTRUCT(BlueprintType)
struct FMissionObjectiveInfo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    bool bTest; 

    // TODO: Main target parameters
};

USTRUCT(BlueprintType)
struct FMissionObjectiveLayout
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    bool bDisplay;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    bool bDisplayProgress;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    FText Title;

    // TODO: Add hints

    FMissionObjectiveLayout() : bDisplay(false), bDisplayProgress(false)
    {
    }
};

USTRUCT(BlueprintType)
struct FMissionObjective
{
    GENERATED_BODY();

    /** Index in current mission progress */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
    int32 Index;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    EMissionObjectiveType Type;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    EMissionStatus Status;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    FMissionObjectiveLayout Layout;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    TArray<FMissionTarget> Targets;

    FMissionObjective()
        : Index(0), Type(EMissionObjectiveType::SingleTarget),
          Status(EMissionStatus::Pending)
    {
    }
};

USTRUCT(BlueprintType)
struct FMissionSequence
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    FText Title;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    TArray<FMissionObjective> Objectives;

    FMissionSequence()
    {}
};

USTRUCT(BlueprintType)
struct FMissionOnFinished
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    bool bStartNextMission;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    FName NextMissionName;

    FMissionOnFinished(): bStartNextMission(false)
    {
    }
};

USTRUCT(BlueprintType)
struct FMissionMaster : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    EMissionType Type;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    FMissionObjectiveInfo MainInfo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    TArray<FMissionSequence> Sequences;

    /** List of targets which will be global for the mission. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    TArray<FMissionTarget> GlobalTargets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
    FMissionOnFinished OnMissionFinished;

    FMissionMaster(): Type(EMissionType::All), MainInfo()
    {
    }
};

USTRUCT(BlueprintType)
struct FMissionTargetUpdatedParams
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite);
    FMissionTarget Target;
    
    UPROPERTY(BlueprintReadWrite);
    EMissionStatus OldStatus;

    UPROPERTY(BlueprintReadWrite);
    int32 OldAmount; 
};