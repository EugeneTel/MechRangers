// Copyright PlatoSpace.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mech/MRMech.h"

#include "MRMechCockpit.generated.h"

class USphereComponent;

/**
 * General class for all Mech Cockpits
 */
UCLASS(Abstract)
class MECHRANGERS_API AMRMechCockpit : public AActor
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	AMRMechCockpit();

protected:

	UPROPERTY(BlueprintReadWrite)
	AMRMech* OwnerMech;

	/** The main skeletal mesh associated with this Cockpit. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* MeshComponent;
};
