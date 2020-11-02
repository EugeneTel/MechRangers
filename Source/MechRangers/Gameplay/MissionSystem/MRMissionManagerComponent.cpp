// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMissionManagerComponent.h"
#include "Log.h"
#include "MechRangers/Modes/MRGameMode.h"

void UMRMissionManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    InitMission();

    StartSequence();
}

//----------------------------------------------------------------------------------------------------------------------
// Missions
//----------------------------------------------------------------------------------------------------------------------

void UMRMissionManagerComponent::InitMission()
{   
    if (UWorld* World = GetWorld())
    {
        AMRGameMode* GameMode = Cast<AMRGameMode>(World->GetAuthGameMode());
        if (GameMode)
        {
            MissionDataTable = GameMode->GetMissionDataTable();
        }
    }

    if (!MissionDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Mission table is not set or can't be initialized!"));
        return;
    }

    const FString ContextString;
    FMissionMaster* MissionMaster = MissionDataTable->FindRow<FMissionMaster>(FName(TEXT("Mission1")), ContextString);

    if (!MissionMaster)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't load first mission"));
        return;
    }

    CurrentMission = *MissionMaster;

    PopulateObjectives();
    PopulateTargets();
}

void UMRMissionManagerComponent::StartSequence()
{
    if (CurrentMission.Sequences.Num() <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Can't start a mission. CurrentMission is not set!"));
        return;
    }

    CurrentSequenceIndex = 0;
    bIsActiveMission = true;
}

void UMRMissionManagerComponent::EnableMissionActors()
{
}

void UMRMissionManagerComponent::MissionFailed()
{
    ULog::Error("------------Mission Failed-------------", LO_Both);
    
    if (bIsActiveMission == false)
        return;

    // TODO: Inform display

    // TODO: Clear Objective Markers

    ClearActiveMission();

    // TODO: Restart mission if needed
}

void UMRMissionManagerComponent::MissionComplete()
{
    ULog::Success("------------Mission Complete-------------", LO_Both);

    ClearActiveMission();
}

void UMRMissionManagerComponent::ClearActiveMission()
{
    bIsActiveMission = false;
    CurrentMission = {};
    CurrentObjectives.Empty();
    CurrentTargets.Empty();
    CurrentSequenceIndex = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Objectives
//----------------------------------------------------------------------------------------------------------------------

void UMRMissionManagerComponent::PopulateObjectives()
{
    if (!CurrentMission.Sequences.IsValidIndex(CurrentSequenceIndex))
        return;

    for (FMissionObjective& Objective : CurrentMission.Sequences[CurrentSequenceIndex].Objectives)
    {
        CurrentObjectives.Add(&Objective);
    }

    // TODO: Add Objective Markers
}

FMissionObjective* UMRMissionManagerComponent::FindObjectiveByTarget(FMissionTarget& InTarget)
{
    if (!CurrentMission.Sequences.IsValidIndex(CurrentSequenceIndex))
        return nullptr;
    
    for (FMissionObjective& Objective : CurrentMission.Sequences[CurrentSequenceIndex].Objectives)
    {
        if (Objective.Targets.FindByKey(InTarget))
        {
            return &Objective;
        }
    }
    
    return nullptr;
}

void UMRMissionManagerComponent::ObjectiveFailed()
{
    ULog::Error("------------Objective Failed-------------", LO_Both);

    MissionFailed();
}

void UMRMissionManagerComponent::ObjectiveComplete()
{
    ULog::Success("------------Objective Complete-------------", LO_Both);

    MissionComplete();
}

void UMRMissionManagerComponent::CheckAndHandleObjectiveComplete(FMissionObjective& Objective)
{
    // Go through all targets and checking if any of targets is not completed
    for (FMissionTarget& Target : Objective.Targets)
    {
        if (Target.Status != EMissionStatus::Completed)
        {
            return;
        }
    }

    ObjectiveComplete();
}

//----------------------------------------------------------------------------------------------------------------------
// Targets
//----------------------------------------------------------------------------------------------------------------------

void UMRMissionManagerComponent::UpdateTargetProgress(FMissionTargetInfo& TargetInfo, EMissionTargetAction const TargetAction)
{
    if (!bIsActiveMission)
        return;
   
    // Find the Target by TargetInfo
    if (FMissionTarget* Target = FindTargetByInfo(TargetInfo))
    {
        // Check Success Action of the Target
        if (Target->SuccessOnActions.Find(TargetAction) != INDEX_NONE)
        {
            TargetProgressSucceed(*Target, TargetInfo);
        }
        // Check Failed Action of the Target
        else if (Target->FailedOnActions.Find(TargetAction) != INDEX_NONE)
        {
            TargetProgressFailed(*Target, TargetInfo);
        }
    }
}

void UMRMissionManagerComponent::PopulateTargets()
{
    for (FMissionObjective* Objective : CurrentObjectives)
    {
        if (Objective->Targets.Num() > 0)
        {
            for (FMissionTarget& Target : Objective->Targets)
            {
                CurrentTargets.Add(&Target);
            }
        }
    }
}

FMissionTarget* UMRMissionManagerComponent::FindTargetByInfo(const FMissionTargetInfo& TargetInfo)
{
    for (FMissionTarget* CurrentTarget : CurrentTargets)
    {
        if (CurrentTarget->bTargetIDMatters && CurrentTarget->TargetID == TargetInfo.TargetID)
        {
            // if ID and Class requires
            if (CurrentTarget->bActorClassMatters == false || (CurrentTarget->bActorClassMatters && CurrentTarget->ActorClass == TargetInfo.ActorClass))
            {
                return CurrentTarget;
            }
        } else if (CurrentTarget->bActorClassMatters && CurrentTarget->ActorClass == TargetInfo.ActorClass)
        {
            return CurrentTarget;
        }
    }
    return nullptr;
}

void UMRMissionManagerComponent::TargetProgressFailed(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo)
{
    TargetFailed(Target, TargetInfo);

    // TODO: Implement additional functionality
}

void UMRMissionManagerComponent::TargetProgressSucceed(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo)
{
    ULog::Success("------------Target Progress Succeed-------------", LO_Both);
    
    Target.CurrentAmount += 1;

    if (Target.CurrentAmount >= Target.Amount)
    {
        TargetComplete(Target, TargetInfo);
    }

    // TODO: Notify target progress succeed
}

void UMRMissionManagerComponent::TargetComplete(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo)
{
    ULog::Success("------------Target Complete-------------", LO_Both);
    
    Target.Status = EMissionStatus::Completed;

    if (FMissionObjective* Objective = FindObjectiveByTarget(Target))
    {
        CheckAndHandleObjectiveComplete(*Objective);
    }
}

void UMRMissionManagerComponent::TargetFailed(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo)
{
    Target.Status = EMissionStatus::Failed;

    ObjectiveFailed();
}
