// Copyright PlatoSpace.com All Rights Reserved.

#include "MRGameHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "MechRangers/Modes/MRGameMode.h"
#include "MechRangers/Gameplay/Characters/Pilot/PilotCharacter.h"
#include "Engine/World.h"
#include "MechRangers/Gameplay/Mech/MRMech.h"
#include "Log.h"

AMRGameHUD::AMRGameHUD()
{   
    // Set the crosshair texture
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/MechRangers/UI/HUD/Textures/Crosshair-blue.Crosshair-blue'"));
    CrosshairTex = CrosshairTexObj.Object;
}

void AMRGameHUD::BeginPlay()
{
    UWorld* World = GetWorld();
    if (World)
    {
        AMRGameMode* GameMode = Cast<AMRGameMode>(World->GetAuthGameMode());
    
        bVRMode = GameMode->IsVR();
    }
}

void AMRGameHUD::DrawHUD()
{
    Super::DrawHUD();

    if (Canvas == nullptr)
        return;
}
