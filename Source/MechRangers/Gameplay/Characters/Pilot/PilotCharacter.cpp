// Fill out your copyright notice in the Description page of Project Settings.


#include "PilotCharacter.h"

APilotCharacter::APilotCharacter()
{
    bVRMode = true;

    FlatCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FlatCamera"));
    FlatCamera->SetupAttachment(GetRootComponent());
    FlatCamera->SetAutoActivate(false);

    VRReplicatedCamera->SetAutoActivate(false);
}

void APilotCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Set OwnedPilot to Mech
    if (OwningMech)
    {
        OwningMech->PilotSitDown(this);
    }
    
    // Set Tracking to Floor. Without it in Oculus Rift is under floor.
    if (bVRMode)
    {
        VRReplicatedCamera->SetAutoActivate(true);
        UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
    } else
    {
        FlatCamera->SetAutoActivate(true);
    }

    

}
