// Copyright PlatoSpace.com All Rights Reserved.

#include "Pilot/MRMechControlComponent.h"
#include "Log.h"

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
	WeaponSystem = Mech->GetWeaponSystem();
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

bool UMRMechControlComponent::IsManipulatorLeftHeld() const
{
	return Mech->IsManipulatorLeftHeld();
}

bool UMRMechControlComponent::IsManipulatorRightHeld() const
{
	return Mech->IsManipulatorRightHeld();
}

void UMRMechControlComponent::PrimaryLeftWeaponStart() const
{
	if (WeaponSystem->CanFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Primary))
	{
		WeaponSystem->StartWeaponFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Primary);
	}
}

void UMRMechControlComponent::PrimaryLeftWeaponFinish() const
{
	WeaponSystem->StopWeaponFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Primary);
}

void UMRMechControlComponent::PrimaryRightWeaponStart() const
{
	if (WeaponSystem->CanFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Primary))
	{
		WeaponSystem->StartWeaponFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Primary);
	}
}

void UMRMechControlComponent::PrimaryRightWeaponFinish() const
{
	WeaponSystem->StopWeaponFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Primary);
}

void UMRMechControlComponent::SecondaryLeftWeaponStart() const
{
	if (WeaponSystem->CanFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Secondary))
	{
		WeaponSystem->StartWeaponFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Secondary);
	}
}

void UMRMechControlComponent::SecondaryLeftWeaponFinish() const
{
	WeaponSystem->StopWeaponFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Secondary);
}

void UMRMechControlComponent::SecondaryRightWeaponStart() const
{
	if (WeaponSystem->CanFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Secondary))
	{
		WeaponSystem->StartWeaponFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Secondary);
	}
}

void UMRMechControlComponent::SecondaryRightWeaponFinish() const
{
	WeaponSystem->StopWeaponFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Secondary);
}
