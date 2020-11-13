// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MRMissionTypes.h"
#include "MRMissionTargetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMissionTargetComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// ctor
	UMRMissionTargetComponent();

	// UActorComponent interface
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	UPROPERTY(Category=MissionActor, EditAnywhere, BlueprintReadWrite)
	FMissionTargetInfo TargetInfo;
	

protected:

	/** Register target in a Mission Manager */
	// UFUNCTION(BlueprintCallable)
	// void RegisterTarget();

	UFUNCTION(BlueprintCallable)
	void OnActorDeath(AActor* DeadActor);

	UFUNCTION(BlueprintCallable)
	void InformTargetProgress(EMissionTargetAction TargetAction);
			
};
