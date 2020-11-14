// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MRMechMissionDisplay.generated.h"

class UMRDisplayWidgetComponent;

UCLASS()
class MECHRANGERS_API AMRMechMissionDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMRMechMissionDisplay();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	
	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MissionDisplay)
	UStaticMeshComponent* DisplayMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MissionDisplay)
	UMRDisplayWidgetComponent* DisplayWidget;

};
