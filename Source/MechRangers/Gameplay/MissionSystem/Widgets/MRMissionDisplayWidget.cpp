// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMissionDisplayWidget.h"
#include "Log.h"
#include "MechRangers/Modes/MRGameMode.h"
#include "MRMissionSingleObjectiveWidget.h"
#include "MechRangers/Gameplay/MissionSystem/MRMissionManagerComponent.h"

void UMRMissionDisplayWidget::NativeConstruct()
{
    if (UWorld* World = GetWorld())
    {
        AMRGameMode* GameMode = Cast<AMRGameMode>(World->GetAuthGameMode());
        if (GameMode)
        {
            MissionManager = GameMode->GetMissionManager();
        }
    }

    if (!MissionManager)
    {
        UE_LOG(LogTemp, Error, TEXT("MissionDisplayWidget::NativeConstruct. MissionManager is not set!"))
    }

    MissionManager->OnMissionInit.AddDynamic(this, &UMRMissionDisplayWidget::OnMissionInit);
    MissionManager->OnMissionFailed.AddDynamic(this, &UMRMissionDisplayWidget::OnMissionFailed);
    MissionManager->OnMissionComplete.AddDynamic(this, &UMRMissionDisplayWidget::OnMissionComplete);
    MissionManager->OnSequenceStarted.AddDynamic(this, &UMRMissionDisplayWidget::OnSequenceStarted);
    MissionManager->OnTargetUpdated.AddDynamic(this, &UMRMissionDisplayWidget::OnTargetUpdated);
    MissionManager->OnObjectiveCompleted.AddDynamic(this, &UMRMissionDisplayWidget::OnObjectiveCompleted);
}

void UMRMissionDisplayWidget::CreateObjectiveWidgets(TArray<FMissionObjective> InObjectives)
{
    if (!ObjectiveWidgetClass || InObjectives.Num() <= 0)
        return;
    
    for( FMissionObjective& Objective : InObjectives)
    {
        UMRMissionSingleObjectiveWidget* NewObjectiveWidget = CreateWidget<UMRMissionSingleObjectiveWidget>(this, ObjectiveWidgetClass);
        if (NewObjectiveWidget)
        {
            NewObjectiveWidget->InitObjective(Objective);
            ObjectivesPanel->AddChild(NewObjectiveWidget);
            ObjectiveWidgets.Add(NewObjectiveWidget);
            NewObjectiveWidget->NotifyDisplayWidget();
        }
    }
}

void UMRMissionDisplayWidget::CleanObjectiveWidgets()
{
    if (ObjectiveWidgets.Num() <= 0)
        return;

    ObjectivesPanel->ClearChildren();
    ObjectiveWidgets.Empty();
}

void UMRMissionDisplayWidget::OnMissionInit(FMissionMaster& NewMission)
{
    //NotifyMissionInit(NewMission);
    //ULog::Success("----------------On Mission Init-------------", LO_Both);
}

void UMRMissionDisplayWidget::OnMissionFailed()
{
    CleanObjectiveWidgets();

    NotifyMissionFailed();
}

void UMRMissionDisplayWidget::OnMissionComplete()
{
    CleanObjectiveWidgets();

    NotifyMissionComplete();
}

void UMRMissionDisplayWidget::OnSequenceStarted(const int32 SequenceIndex)
{
    if (MissionManager)
    {
        CleanObjectiveWidgets();
        
        CurrentSequence = *MissionManager->GetCurrentSequence();

        CreateObjectiveWidgets(CurrentSequence.Objectives);

        NotifyUpdateSequenceTexts();
    }

    NotifySequenceStarted(SequenceIndex);
    
    //ULog::Success("----------------On Sequence Started-------------", LO_Both);
}

void UMRMissionDisplayWidget::OnTargetUpdated(FMissionTargetUpdatedParams Params)
{

    for (UMRMissionSingleObjectiveWidget* ObjectiveWidget : ObjectiveWidgets)
    {
        // Try to find and update a target
        if (ObjectiveWidget->CheckAndHandleTargetProgress(Params))
        {
            // Stop if found
            break;
        }
    }
}

void UMRMissionDisplayWidget::OnObjectiveCompleted(const int32 ObjectiveIndex)
{
    // Try to find objective and update status
    for (UMRMissionSingleObjectiveWidget* ObjectiveWidget : ObjectiveWidgets)
    {
        if (ObjectiveWidget->CheckAndHandleObjectiveCompleted(ObjectiveIndex))
        {
            // Stop if found
            break;
        }
    }
}
