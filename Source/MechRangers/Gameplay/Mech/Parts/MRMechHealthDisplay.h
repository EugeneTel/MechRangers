// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechRangers/UI/Widgets/MRDisplayWidgetComponent.h"

#include "MRMechHealthDisplay.generated.h"

class UWidgetComponent;

UCLASS()
class MECHRANGERS_API AMRMechHealthDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMRMechHealthDisplay();

protected:

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HealthDisplay)
	UStaticMeshComponent* DisplayMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HealthDisplay)
	UMRDisplayWidgetComponent* DisplayWidget;

};
