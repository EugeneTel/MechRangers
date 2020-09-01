// Copyright PlatoSpace.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MRMechCockpit.generated.h"

/**
 * General class for all Mech Cockpits
 */
UCLASS(Abstract)
class MECHRANGERS_API AMRMechCockpit : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AMRMechCockpit();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};