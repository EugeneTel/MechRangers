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

void UMRMechControlComponent::MoveForward(const float Val) const
{
	Mech->MoveForward(Val);
}

void UMRMechControlComponent::TurnAtRate(const float Val) const
{
	Mech->TurnAtRate(Val);
}

void UMRMechControlComponent::SitPilotIntoMech(AMRMech* NewMech)
{
	Mech = NewMech;
}

void UMRMechControlComponent::SetCombatMode(const bool Val)
{
	Mech->SetCombatMode(Val);
}

bool UMRMechControlComponent::IsCombatMode() const
{
	return Mech->IsCombatMode();
}

//----------------------------------------------------------------------------------------------------------------------
// Arm Controls
//----------------------------------------------------------------------------------------------------------------------

void UMRMechControlComponent::AddArmLeftPitch(const float Val) const
{
	Mech->AddArmLeftRotator(FRotator(Val, 0.f, 0.f));
}

void UMRMechControlComponent::AddArmLeftYaw(const float Val) const
{
	Mech->AddArmLeftRotator(FRotator(0.f, Val, 0.f));
}

void UMRMechControlComponent::AddArmRightPitch(const float Val) const
{
	Mech->AddArmRightRotator(FRotator(Val, 0.f, 0.f));
}

void UMRMechControlComponent::AddArmRightYaw(const float Val) const
{
	Mech->AddArmRightRotator(FRotator(0.f, Val, 0.f));
}
