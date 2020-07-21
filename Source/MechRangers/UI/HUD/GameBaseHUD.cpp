// Fill out your copyright notice in the Description page of Project Settings.


#include "GameBaseHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "MechRangers/Core/MechRangersGameMode.h"
#include "MechRangers/Gameplay/Characters/Pilot/PilotCharacter.h"
#include "Engine/World.h"
#include "MechRangers/Gameplay/Mechs/BaseMech.h"
#include "Log.h"

AGameBaseHUD::AGameBaseHUD()
{   
    // Set the crosshair texture
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/MechRangers/UI/HUD/Textures/Crosshair-blue.Crosshair-blue'"));
    CrosshairTex = CrosshairTexObj.Object;
}

void AGameBaseHUD::BeginPlay()
{
    UWorld* World = GetWorld();
    if (World)
    {
        AMechRangersGameMode* GameMode = Cast<AMechRangersGameMode>(World->GetAuthGameMode());
    
        bVRMode = GameMode->bVRMode;
    }
}

void AGameBaseHUD::DrawHUD()
{
    Super::DrawHUD();

    if (Canvas == nullptr)
        return;

   // DrawLimbsCrosshairs();
}

void AGameBaseHUD::DrawCrosshairFlat(FVector CrosshairLocation, ELimbCrosshairType CrosshairType)
{
    if (CrosshairType == ELimbCrosshairType::ELCT_None)
        return;

    FVector ScreenLocation;
    GetOwningPlayerController()->ProjectWorldLocationToScreenWithDistance(CrosshairLocation, ScreenLocation, true);

    // Calculate crosshair size
    float CrosshairMaxSize = 80.f;
    float CrosshairMinSize = 30.f;
    float DistanceAlpha = ScreenLocation.Z / 10000;
    float CrosshairSize = FMath::Lerp(CrosshairMaxSize, CrosshairMinSize, DistanceAlpha);
    
    FVector2D Position (ScreenLocation.X - (CrosshairSize / 2), ScreenLocation.Y - (CrosshairSize / 2));
    
    // draw the crosshair
    FCanvasTileItem TileItem( Position, CrosshairTex->Resource, FLinearColor::White);
    TileItem.Size = FVector2D(CrosshairSize, CrosshairSize);
    TileItem.BlendMode = SE_BLEND_Translucent;
   
    Canvas->DrawItem( TileItem );
}

void AGameBaseHUD::DrawLimbsCrosshairs()
{  
    APilotCharacter* PilotCharacter = Cast<APilotCharacter>(GetOwningPawn());
    if (!PilotCharacter || !PilotCharacter->GetController())
        return;
    
    ABaseMech* OwningMech = PilotCharacter->GetOwningMech();
    if (!OwningMech)
        return;
        
    // Go through all limbs, get Trace End Point for Crosshair drawing 
    TMap<ELimbType, ABaseLimb*> MechLimbs = OwningMech->GetLimbs();
    for (const TPair<ELimbType, ABaseLimb*>& LimbItem : MechLimbs)
    {
        ABaseLimb* Limb = LimbItem.Value;
        if (Limb && Limb->GetCrosshairType() != ELimbCrosshairType::ELCT_None)
        {
            FVector TraceEndPoint = Limb->GetTraceEndPoint();

            if (bVRMode)
            {
                
            } else
            {
                DrawCrosshairFlat(TraceEndPoint, Limb->GetCrosshairType());
            }
        }
    }
}
