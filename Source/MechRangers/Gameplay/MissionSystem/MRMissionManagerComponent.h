// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MRMissionTypes.h"
#include "Components/ActorComponent.h"
#include "MRMissionManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMissionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// ctor
	// UMRMissionManagerComponent();

	// UActorComponent interface
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdateTargetProgress(FMissionTargetInfo& TargetInfo, EMissionTargetAction const TargetAction);

protected:

	UPROPERTY(Category=MissionManager, VisibleInstanceOnly, BlueprintReadWrite)
	bool bIsActiveMission = false;

	UPROPERTY(BlueprintReadWrite)
	UDataTable* MissionDataTable = nullptr;

	UPROPERTY(Category=MissionManager, VisibleInstanceOnly, BlueprintReadWrite)
	FMissionMaster CurrentMission;

	TArray<FMissionTarget*> CurrentTargets;

	int32 CurrentSequenceIndex = 0;

	TArray<FMissionObjective*> CurrentObjectives;

//----------------------------------------------------------------------------------------------------------------------
// Missions
//----------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable)
	void InitMission();

	UFUNCTION(BlueprintCallable)
	void StartSequence();
	
	/** Enables actors for the current mission */
	UFUNCTION(BlueprintCallable)
	void EnableMissionActors();
	
	/** Called when a Mission is failed */
	UFUNCTION(BlueprintCallable)
    void MissionFailed();

	/** Called when a Mission is successfully complete */
	UFUNCTION(BlueprintCallable)
    void MissionComplete();

	UFUNCTION(BlueprintCallable)
    void ClearActiveMission();

//----------------------------------------------------------------------------------------------------------------------
// Objectives
//----------------------------------------------------------------------------------------------------------------------

	/** Save Current Sequence's Objectives In a Variable */
	UFUNCTION(BlueprintCallable)
    void PopulateObjectives();

	FMissionObjective* FindObjectiveByTarget(FMissionTarget& InTarget);

	/** Called when an Objective is failed */
	UFUNCTION(BlueprintCallable)
	void ObjectiveFailed();

	/** Called when an Objective is successfully complete */
	UFUNCTION(BlueprintCallable)
	void ObjectiveComplete();

	UFUNCTION(BlueprintCallable)
	void CheckAndHandleObjectiveComplete(FMissionObjective& Objective);

//----------------------------------------------------------------------------------------------------------------------
// Targets
//----------------------------------------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable)
	void PopulateTargets();

	FMissionTarget* FindTargetByInfo(const FMissionTargetInfo& TargetInfo);

	/** Single progress failed */
	UFUNCTION(BlueprintCallable)
	void TargetProgressFailed(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo);

	/** Succeed single progress */
	UFUNCTION(BlueprintCallable)
	void TargetProgressSucceed(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo);

	/** All amounts succeeded */
	UFUNCTION(BlueprintCallable)
	void TargetComplete(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo);

	/** Target Failed */
	UFUNCTION(BlueprintCallable)
	void TargetFailed(FMissionTarget& Target, const FMissionTargetInfo& TargetInfo);

	
		
};
