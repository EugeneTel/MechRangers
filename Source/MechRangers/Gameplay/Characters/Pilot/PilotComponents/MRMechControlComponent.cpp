// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMechControlComponent.h"

// Sets default values for this component's properties
UMRMechControlComponent::UMRMechControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UMRMechControlComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMRMechControlComponent::MoveForward(float Val)
{
	if (!Mech)
		return;

	Mech->MoveForward(Val);
}

void UMRMechControlComponent::TurnAtRate(float Val)
{
	if (!Mech)
		return;

	Mech->TurnAtRate(Val);
}

void UMRMechControlComponent::SitPilotIntoMech(AMRMech* NewMech)
{
	Mech = NewMech;
}
