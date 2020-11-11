// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMechAimComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../MRMech.h"
#include "../UI/MRCrosshair.h"
#include "../Cockpit/MRMechCockpit.h"

UMRMechAimComponent::UMRMechAimComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickInterval = 0.f;
    
    CrosshairType = ECrosshairType::ECT_None;
}

void UMRMechAimComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache mech
    OwnerMech = Cast<AMRMech>(GetOwner());

    if (CrosshairType != ECrosshairType::ECT_None && OwnerMech->HasPilot())
    {
        SpawnCrosshair();
    }
}

void UMRMechAimComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateCrosshair();
}

void UMRMechAimComponent::SpawnCrosshair()
{
    if (!CrosshairClass)
        return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Instigator = OwnerMech;
    SpawnParams.Owner = OwnerMech;
    Crosshair = GetWorld()->SpawnActor<AMRCrosshair>(CrosshairClass, GetComponentLocation(), FRotator::ZeroRotator, SpawnParams);
    Crosshair->AttachToActor(OwnerMech, FAttachmentTransformRules::KeepWorldTransform);
}

void UMRMechAimComponent::UpdateCrosshair() const
{
    // Update Crosshair Position
    if (Crosshair && !TraceEndPoint.IsZero())
    {
        Crosshair->SetActorLocation(TraceEndPoint);

        // Rotate crosshair to the Pilot
        Crosshair->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Crosshair->GetActorLocation(), OwnerMech->Cockpit->GetActorLocation()));
    }
}
