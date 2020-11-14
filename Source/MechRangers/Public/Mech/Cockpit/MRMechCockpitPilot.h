// Copyright PlatoSpace.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MRMechCockpit.h"
#include "MRMechCockpitPilot.generated.h"

class USphereComponent;

UCLASS()
class MECHRANGERS_API AMRMechCockpitPilot : public AMRMechCockpit
{
	GENERATED_BODY()

public:

	AMRMechCockpitPilot();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
    USceneComponent* GetPilotAttachmentPoint() const;

	UFUNCTION(BlueprintCallable)
    USphereComponent* GetHeadZoneVisualizer() const;

protected:
	/** Where a pilot should be attached */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* PilotAttachmentPoint;

	/** OwnedPilot head zone visualization */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* HeadZoneVisualizer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChildActorComponent* HealthDisplayContainer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChildActorComponent* MissionDisplayContainer;

	void SetupHealthDisplay() const;
};
