// Copyright PlatoSpace.com All Rights Reserved.

#include "MissionSystem/MRMissionManagerComponent.h"
#include "Log.h"
#include "Framework/MRGameMode.h"

void UMRMissionManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle TmpHandler;
    GetWorld()->GetTimerManager().SetTimer(TmpHandler, this, &UMRMissionManagerComponent::InitMission, 1.f);
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

    if (OnMissionInit.IsBound())
    {
        OnMissionInit.Broadcast(CurrentMission);
    }

    StartSequence();
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

    if (OnSequenceStarted.IsBound())
    {
        OnSequenceStarted.Broadcast(CurrentSequenceIndex);
    }
}

void UMRMissionManagerComponent::EnableMissionActors()
{
}

void UMRMissionManagerComponent::MissionFailed()
{
    ULog::Error("------------Mission Failed-------------", LO_Both);
    
    if (bIsActiveMission == false)
        return;

    if (OnMissionFailed.IsBound())
    {
        OnMissionFailed.Broadcast();
    }

    // TODO: Clear Objective Markers

    ClearActiveMission();

    // TODO: Restart mission if needed
}

void UMRMissionManagerComponent::MissionComplete()
{
    ULog::Success("------------Mission Complete-------------", LO_Both);

    if (OnMissionComplete.IsBound())
    {
        OnMissionComplete.Broadcast();
    }

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

    int32 ObjectiveIndex = 0;

    for (FMissionObjective& Objective : CurrentMission.Sequences[CurrentSequenceIndex].Objectives)
    {
        Objective.Index = ObjectiveIndex;
        Objective.Status = EMissionStatus::InProgress;
        CurrentObjectives.Add(&Objective);
        ObjectiveIndex++;
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

void UMRMissionManagerComponent::CheckAndHandleObjectiveComplete(FMissionObjective& Objective)
{
    if (Objective.Status != EMissionStatus::InProgress)
        return;
    
    // Go through all targets and checking if any of targets is not completed
    for (FMissionTarget& Target : Objective.Targets)
    {
        if (Target.Status != EMissionStatus::Completed)
        {
            return;
        }
    }

    // If all Targets are complete then Objective is complete
    Objective.Status = EMissionStatus::Completed;

    if (OnObjectiveCompleted.IsBound())
    {
        OnObjectiveCompleted.Broadcast(Objective.Index);
    }

    CheckAndHandleSequenceComplete();    
}

void UMRMissionManagerComponent::CheckAndHandleSequenceComplete()
{
    FMissionSequence* Sequence = GetCurrentSequence();
    if (!Sequence)
        return;

    for (const auto Objective : Sequence->Objectives)
    {
        if (Objective.Status != EMissionStatus::Completed)
        {
            return;
        }
    }

    // If all Objectives are complete then Sequence is complete
    // TODO: Implement next sequence functionality

    MissionComplete();
}

//----------------------------------------------------------------------------------------------------------------------
// Targets
//----------------------------------------------------------------------------------------------------------------------

void UMRMissionManagerComponent::UpdateTargetProgress(const FMissionTargetInfo TargetInfo, EMissionTargetAction const TargetAction)
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

FMissionSequence* UMRMissionManagerComponent::GetCurrentSequence()
{
    if (CurrentMission.Sequences.IsValidIndex(CurrentSequenceIndex))
    {
        return &CurrentMission.Sequences[CurrentSequenceIndex];
    }

    return nullptr;
}

void UMRMissionManagerComponent::PopulateTargets()
{
    // Add objective targets
    for (FMissionObjective* Objective : CurrentObjectives)
    {
        if (Objective->Targets.Num() > 0)
        {
            for (FMissionTarget& Target : Objective->Targets)
            {
                Target.Status = EMissionStatus::InProgress;
                CurrentTargets.Add(&Target);
            }
        }
    }

    // Add global targets
    if (CurrentMission.GlobalTargets.Num() > 0)
    {
        for (FMissionTarget& Target : CurrentMission.GlobalTargets)
        {
            Target.Status = EMissionStatus::InProgress;
            CurrentTargets.Add(&Target);
        }
    }

}

FMissionTarget* UMRMissionManagerComponent::FindTargetByInfo(const FMissionTargetInfo& TargetInfo)
{
    for (FMissionTarget* CurrentTarget : CurrentTargets)
    {
        if (CurrentTarget->Status != EMissionStatus::InProgress)
            continue;
        
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
    Target.Status = EMissionStatus::Failed;

    if (Target.bMandatory)
    {
        MissionFailed();
    }

    // TODO: Implement Objective failing
}

void UMRMissionManagerComponent::TargetProgressSucceed(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo)
{
    if (Target.Status != EMissionStatus::InProgress)
        return;

    const int32 OldAmount = Target.Amount;
    const EMissionStatus OldStatus = Target.Status;
    
    Target.CurrentAmount += 1;

    // Target Complete
    if (Target.CurrentAmount >= Target.Amount)
    {
        Target.Status = EMissionStatus::Completed;
    }

    // Notify subscribers
    if (OnTargetUpdated.IsBound())
    {
        const FMissionTargetUpdatedParams TargetUpdatedParams {
            Target,
            OldStatus,
            OldAmount
        };
        
        OnTargetUpdated.Broadcast(TargetUpdatedParams);
    }

    // Check is objective completed
    if (Target.Status == EMissionStatus::Completed)
    {
        if (FMissionObjective* Objective = FindObjectiveByTarget(Target))
        {
            CheckAndHandleObjectiveComplete(*Objective);
        }
    }
}
