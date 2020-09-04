// Copyright PlatoSpace.com All Rights Reserved.

#include "MRPilotFP.h"
#include "PilotComponents/MRMechControlComponent.h"
#include "MechRangers/Gameplay/Mech/MRMechCockpit.h"
#include "Log.h"

// Sets default values
AMRPilotFP::AMRPilotFP()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create Components
	MechControl = CreateDefaultSubobject<UMRMechControlComponent>(TEXT("MechControlComponent"));
}

// Called when the game starts or when spawned
void AMRPilotFP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AMRPilotFP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMRPilotFP::MechMoveForward);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMRPilotFP::MechTurnAtRate);

}

//----------------------------------------------------------------------------------------------------------------------
// Mech Control Wrapper
//----------------------------------------------------------------------------------------------------------------------

UMRMechControlComponent* AMRPilotFP::GetMechControlComponent() const
{
	return MechControl;
}

void AMRPilotFP::SitIntoMech_Implementation(AMRMech* NewMech)
{
	if (NewMech->Cockpit)
	{
		AttachToComponent(NewMech->Cockpit->GetPilotAttachmentPoint(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	} else
	{
		UE_LOG(LogActor, Warning, TEXT("SitIntoMech: Can't sit a Pilot into a Mech. Cockpit is not set!"));
	}

	MechControl->SitPilotIntoMech(NewMech);
}

void AMRPilotFP::MechMoveForward(float Val)
{
	if (Val != 0)
	{
		MechControl->MoveForward(Val);
	}
}

void AMRPilotFP::MechTurnAtRate(float Val)
{
	if (Val != 0)
	{
		MechControl->TurnAtRate(Val);
	}
}

