// Copyright PlatoSpace.com All Rights Reserved.

#include "Turret/MRTurret_Mechanic.h"
#include "Log.h"
#include "Gameplay/MRDestructiblePieceComponent.h"

AMRTurret_Mechanic::AMRTurret_Mechanic(const class FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    TurretBase = CreateDefaultSubobject<UMRDestructiblePieceComponent>(TEXT("TurretBase"));
    TurretBase->SetupAttachment(RootComponent);
    TurretBase->GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TurretBaseRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretBaseRing"));
    TurretBaseRing->SetupAttachment(TurretBase);
    TurretBaseRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TurretPitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretPitch"));
    TurretPitch->SetupAttachment(TurretBaseRing);
    TurretPitch->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    WeaponsAttachmentPoint->SetupAttachment(TurretPitch);
    AimSystem->SetupAttachment(TurretPitch);
}

void AMRTurret_Mechanic::Damaged()
{
    Super::Damaged();

    // Set damaged destructible component
    TurretBase->Damaged();
}

void AMRTurret_Mechanic::Death()
{
    Super::Death();

    // Destroy Turret
    TurretBase->Destroyed();
    TurretBaseRing->DestroyComponent();
    TurretPitch->DestroyComponent();
    AimSystem->DestroyComponent();
}

void AMRTurret_Mechanic::FollowCombatTarget(float DeltaTime)
{    
    if (IsValid(CombatTarget))
    {
        FVector Dir = (CombatTarget->GetActorLocation() - TurretPitch->GetComponentLocation());
        Dir.Normalize();
        TurretPitch->SetWorldRotation(FMath::RInterpTo(TurretPitch->GetComponentRotation(), Dir.Rotation(), DeltaTime, 3.f));
    }
}
