// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Mech/MRMech.h"

#include "MRUnitManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRUnitManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMRUnitManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	/** Spawn specified mech at the transform */
	UFUNCTION(BlueprintCallable)
	AMRMech* SpawnMech(const TSubclassOf<AMRMech> MechClass, const FMechLoadout& MechLoadout, const FTransform& SpawnTransform);
		
};
