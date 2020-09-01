// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMechAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

void UMRMechAnimInstance::NativeInitializeAnimation()
{
    MechOwner = Cast<AMRMech>(TryGetPawnOwner());
}

void UMRMechAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    if (!MechOwner)
        return;

    // Calculate Mech Speed
    Speed = FVector::DotProduct(MechOwner->GetVelocity(), MechOwner->GetActorForwardVector());

    // TODO: Refactor Turn Speed Calculation
    const float YawDelta = UKismetMathLibrary::NormalizedDeltaRotator(MechOwner->GetActorRotation(), LastKnownRot).Yaw;
    TurnSpeed = 20000.f * DeltaSeconds * YawDelta;
    LastKnownRot = MechOwner->GetActorRotation();
    
}
