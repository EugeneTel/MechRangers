// Copyright PlatoSpace.com All Rights Reserved.


#include "MissionSystem/MRMissionSingleObjectiveWidget.h"

void UMRMissionSingleObjectiveWidget::InitObjective(FMissionObjective& InObjective)
{
    Objective = InObjective;
    Layout = Objective.Layout;
    Completed = false;

    RefreshProgress();

    NotifyDisplayWidget();
}

void UMRMissionSingleObjectiveWidget::RefreshProgress()
{
    if (Objective.Layout.bDisplayProgress == false || Objective.Targets.Num() <= 0)
        return;

    int32 TotalTargets = 0;
    int32 CurrentTargets = 0;

    for (FMissionTarget& Target : Objective.Targets)
    {
        TotalTargets += Target.Amount;
        CurrentTargets += Target.CurrentAmount;
    }

    // Notify if current targets were updated
    const bool ShouldNotify = CurrentTargets != CurrentInProgress;

    MaxInProgress = TotalTargets;
    CurrentInProgress = CurrentTargets;

    if (ShouldNotify)
    {
        NotifyRefreshProgress();
    }
}

bool UMRMissionSingleObjectiveWidget::CheckAndHandleTargetProgress(FMissionTargetUpdatedParams& Params)
{
    if (Objective.Layout.bDisplayProgress == false || Objective.Targets.Num() <= 0)
        return false;
    
    for (FMissionTarget& Target : Objective.Targets)
    {
        if (Target == Params.Target)
        {
            Target.CurrentAmount = Params.Target.CurrentAmount;
            Target.Status = Params.Target.Status;
            RefreshProgress();
            return true;
        }
    }

    return false;
}

bool UMRMissionSingleObjectiveWidget::CheckAndHandleObjectiveCompleted(const int32 ObjectiveIndex)
{
    if (Objective.Index == ObjectiveIndex)
    {
        Objective.Status = EMissionStatus::Completed;
        NotifyObjectiveCompleted();
        return true;
    }

    return false;
}
