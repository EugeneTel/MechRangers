// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechRangers/Gameplay/Components/MRDisplayWidgetComponent.h"
#include "MRMechHealthDisplay.generated.h"

class UWidgetComponent;
class AMRMech;

UCLASS()
class MECHRANGERS_API AMRMechHealthDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMRMechHealthDisplay();

	UFUNCTION(BlueprintCallable)
	void SetOwnerMech(AMRMech* InMech);

protected:

	UPROPERTY(BlueprintReadWrite)
	AMRMech* OwnerMech;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HealthDisplay)
	UStaticMeshComponent* DisplayMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = HealthDisplay)
	UMRDisplayWidgetComponent* DisplayWidget;

};
