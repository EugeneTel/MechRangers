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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AMRMechCockpitPilot();

private:

	/** Where a pilot should be attached */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USceneComponent* PilotAttachmentPoint;

	/** OwnedPilot head zone visualization */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USphereComponent* HeadZoneVisualizer;

public:

	UFUNCTION(BlueprintCallable)
	USceneComponent* GetPilotAttachmentPoint() const;

	UFUNCTION(BlueprintCallable)
	USphereComponent* GetHeadZoneVisualizer() const;
};
