// Copyright PlatoSpace.com All Rights Reserved.

#include "MRPilotVR.h"
#include "PilotComponents/MRMechControlComponent.h"
#include "MechRangers/Gameplay/Mech/MRMechCockpit.h"

AMRPilotVR::AMRPilotVR()
{
   // UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

    // Create Components
    MechControl = CreateDefaultSubobject<UMRMechControlComponent>(TEXT("MechControlComponent"));
}

// Called to bind functionality to input
void AMRPilotVR::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMRPilotVR::MechMoveForward);
    PlayerInputComponent->BindAxis("TurnRate", this, &AMRPilotVR::MechTurnAtRate);

}

//----------------------------------------------------------------------------------------------------------------------
// Mech Control Wrapper
//----------------------------------------------------------------------------------------------------------------------

UMRMechControlComponent* AMRPilotVR::GetMechControlComponent() const
{
    return MechControl;
}

void AMRPilotVR::SitIntoMech_Implementation(AMRMech* NewMech)
{
    if (NewMech->Cockpit)
    {
        const FTransform CameraTransform = VRReplicatedCamera->GetRelativeTransform();
        const FTransform TransformToSeat = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);

        SetSeatedMode(NewMech->Cockpit->GetPilotAttachmentPoint(), true, TransformToSeat, CameraTransform);
        
        //AttachToComponent(NewMech->Cockpit->GetPilotAttachmentPoint(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    } else
    {
        UE_LOG(LogActor, Warning, TEXT("SitIntoMech: Can't sit a Pilot into a Mech. Cockpit is not set!"));
    }

    MechControl->SitPilotIntoMech(NewMech);
}

void AMRPilotVR::MechMoveForward(float Val)
{
    MechControl->MoveForward(Val);
}

void AMRPilotVR::MechTurnAtRate(float Val)
{
    MechControl->TurnAtRate(Val);
}