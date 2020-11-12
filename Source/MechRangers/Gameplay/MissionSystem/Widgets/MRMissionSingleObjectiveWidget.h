// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MechRangers/Gameplay/MissionSystem/MRMissionTypes.h"

#include "MRMissionSingleObjectiveWidget.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRMissionSingleObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void InitObjective(FMissionObjective& InObjective);

	void RefreshProgress();

	/** Try to find and update a Target for the Objective. Returns True if found and updated */
	bool CheckAndHandleTargetProgress(FMissionTargetUpdatedParams& Params);

	bool CheckAndHandleObjectiveCompleted(int32 ObjectiveIndex);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category=MechRangers)
	void NotifyRefreshProgress();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category=MechRangers)
	void NotifyObjectiveCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
	void NotifyDisplayWidget();
	
protected:

	UPROPERTY(BlueprintReadWrite)
	FMissionObjectiveLayout Layout;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxInProgress;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentInProgress;

	UPROPERTY(BlueprintReadWrite)
	bool Completed;

	UPROPERTY(BlueprintReadWrite)
	FMissionObjective Objective;

	

	
};
