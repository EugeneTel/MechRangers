// Copyright PlatoSpace.com All Rights Reserved.

#include "Framework/MRUnitManagerComponent.h"

// Sets default values for this component's properties
UMRUnitManagerComponent::UMRUnitManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMRUnitManagerComponent::BeginPlay()
{
	Super::BeginPlay();

}

AMRMech* UMRUnitManagerComponent::SpawnMech(const TSubclassOf<AMRMech> MechClass, const FMechLoadout& MechLoadout, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.ObjectFlags |= RF_Transient;	// We never want to save Mech pawns into a map
	AMRMech* ResultMech = GetWorld()->SpawnActor<AMRMech>(MechClass, SpawnTransform, SpawnInfo);
	if (!ResultMech)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnMech: Couldn't spawn Mech of type %s at %s"), *GetNameSafe(MechClass), *SpawnTransform.ToHumanReadableString());
	}

	ResultMech->SetLoadout(MechLoadout);
	ResultMech->ConstructMech();

	return ResultMech;
}
