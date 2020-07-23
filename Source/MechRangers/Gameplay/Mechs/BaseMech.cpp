// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMech.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "MechRangers/Gameplay/Characters/Pilot/PilotCharacter.h"

ABaseMech::ABaseMech()
{
    RootReference = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootReference"));
    SetRootComponent(RootReference);

    Cabin = CreateDefaultSubobject<USceneComponent>(TEXT("Cabin"));
    Cabin->SetupAttachment(RootReference);

    /** Setup Pilot */
    PilotAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PilotAttachment"));
    PilotAttachmentPoint->SetupAttachment(Cabin);

    HeadZoneVisualizer = CreateDefaultSubobject<USphereComponent>(TEXT("HeadZoneVisualizer"));
    HeadZoneVisualizer->SetupAttachment(PilotAttachmentPoint);
    HeadZoneVisualizer->SetSphereRadius(40.f);
    HeadZoneVisualizer->SetCollisionProfileName(FName("NoCollision"));
    
}

void ABaseMech::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseMech::LimbControl(const float AxisX, const float AxisY, const ELimbType LimbType)
{
    if (AxisX == 0.f && AxisY == 0.f)
        return;

    ABaseLimb* Limb = GetLimb(LimbType);
    if (!Limb)
    {
        UE_LOG(LogTemp, Warning, TEXT("Requested Limb is not found!!!"));
        return;
    }

    Limb->Control(AxisX, AxisY);
}

//----------------------------------------------------------------------------------------------------------------------
// OwnedPilot Methods
//----------------------------------------------------------------------------------------------------------------------

void ABaseMech::PilotSitDown(APilotCharacter* PilotRef)
{
    if (PilotRef == nullptr)
        return;

    // Setup the OwnedPilot
    OwnedPilot = PilotRef;
    SetOwner(OwnedPilot);

    // The OwnedPilot taking control of the Mech
    const FTransform CameraTransform = PilotRef->VRReplicatedCamera->GetRelativeTransform();
    const FTransform TransformToSeat = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);
    PilotTakingControl(PilotRef, true, PilotAttachmentPoint, TransformToSeat, CameraTransform);
}

void ABaseMech::PilotTakingControl(APilotCharacter* PilotRef, bool bTaking, USceneComponent* AttachPoint,
                                   FTransform RelativeTransformToSeat, FTransform InitialCameraTransform)
{
    PilotRef->SetSeatedMode(AttachPoint, bTaking, RelativeTransformToSeat, InitialCameraTransform);

    // @TODO: Notify OwnedPilot Took Control

    // @TODO: Posses the Mech by the OwnedPilot

}

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------

void ABaseMech::LimbStartFire(const ELimbType LimbType, const ELimbSocket Socket)
{
    if (Limbs.Contains(LimbType) && IsValid(Limbs[LimbType]) && Limbs[LimbType]->CanFire(Socket))
    {
        Limbs[LimbType]->StartWeaponFire(Socket);
    }
}

void ABaseMech::LimbStopFire(const ELimbType LimbType, const ELimbSocket Socket)
{
    if (Limbs.Contains(LimbType) && IsValid(Limbs[LimbType]))
    {
        Limbs[LimbType]->StopWeaponFire(Socket);
    }
}
