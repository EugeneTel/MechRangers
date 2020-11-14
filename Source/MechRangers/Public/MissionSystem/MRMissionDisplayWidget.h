// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/VerticalBox.h"
#include "MissionSystem/MRMissionTypes.h"
#include "UI/MRDisplayWidget.h"
#include "MRMissionDisplayWidget.generated.h"

class UMRMissionManagerComponent;
class UMRMissionSingleObjectiveWidget;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRMissionDisplayWidget : public UMRDisplayWidget
{
	GENERATED_BODY()

protected:

//----------------------------------------------------------------------------------------------------------------------
// Objectives
//----------------------------------------------------------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidgetOptional))
	UVerticalBox* ObjectivesPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMRMissionSingleObjectiveWidget> ObjectiveWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	TArray<UMRMissionSingleObjectiveWidget*> ObjectiveWidgets;

	void CreateObjectiveWidgets(TArray<FMissionObjective> InObjectives);

	void CleanObjectiveWidgets();

//----------------------------------------------------------------------------------------------------------------------
// Sequence
//----------------------------------------------------------------------------------------------------------------------

	UPROPERTY(BlueprintReadWrite)
	FMissionSequence CurrentSequence;
	
	UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
	void NotifyUpdateSequenceTexts();
	
// OLD
protected:

	UPROPERTY(BlueprintReadWrite)
	UMRMissionManagerComponent* MissionManager;

	//UPROPERTY(BlueprintReadWrite)
	FMissionMaster* CurrentMission;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnMissionInit(FMissionMaster& NewMission);

	UFUNCTION()
	void OnMissionFailed();
	
	UFUNCTION()
    void OnMissionComplete();

	UFUNCTION()
	void OnSequenceStarted(int32 SequenceIndex);

	UFUNCTION()
	void OnTargetUpdated(FMissionTargetUpdatedParams Params);
	
	UFUNCTION()
    void OnObjectiveCompleted(int32 ObjectiveIndex);

	UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
	void NotifySequenceStarted(int32 SequenceIndex);

	UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
	void NotifyTargetUpdated(FMissionTargetUpdatedParams Params);

	UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
	void NotifyMissionInit();

	UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
	void NotifyMissionFailed();

	UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
	void NotifyMissionComplete();
	
};
