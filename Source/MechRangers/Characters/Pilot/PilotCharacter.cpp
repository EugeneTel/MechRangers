// Fill out your copyright notice in the Description page of Project Settings.


#include "PilotCharacter.h"

void APilotCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Set Tracking to Floor. Without it in Oculus Rift is under floor.
    UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
}
