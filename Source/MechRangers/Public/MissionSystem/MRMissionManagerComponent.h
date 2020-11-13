// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MRMissionTypes.h"
#include "Components/ActorComponent.h"
#include "MRMissionManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionInitSignature, FMissionMaster&, NewMission);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionCompleteSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMissionFailedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionSequenceStartedSignature, int32, SequenceIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionObjectiveCompletedSignature, int32, ObjectiveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionTargetUpdatedSignature, FMissionTargetUpdatedParams, Params);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMissionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// ctor
	// UMRMissionManagerComponent();

	// UActorComponent interface
	virtual void BeginPlay() override;

	FOnMissionInitSignature OnMissionInit;
	
	UPROPERTY(BlueprintAssignable, Category=MissionManager)
	FOnMissionCompleteSignature OnMissionComplete;
	
	UPROPERTY(BlueprintAssignable, Category=MissionManager)
	FOnMissionFailedSignature OnMissionFailed;
	
	UPROPERTY(BlueprintAssignable, Category=MissionManager)
	FOnMissionTargetUpdatedSignature OnTargetUpdated;
	
	UPROPERTY(BlueprintAssignable, Category=MissionManager)
	FOnMissionSequenceStartedSignature OnSequenceStarted;
		
	UPROPERTY(BlueprintAssignable, Category=MissionManager)
	FOnMissionObjectiveCompletedSignature OnObjectiveCompleted;

	UFUNCTION(BlueprintCallable)
	void UpdateTargetProgress(FMissionTargetInfo TargetInfo, EMissionTargetAction const TargetAction);

	FORCEINLINE FMissionMaster* GetCurrentMission() { return &CurrentMission; }

	FMissionSequence* GetCurrentSequence();

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

	UFUNCTION(BlueprintCallable)
	void CheckAndHandleObjectiveComplete(FMissionObjective& Objective);

	UFUNCTION(BlueprintCallable)
	void CheckAndHandleSequenceComplete();

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
};
