// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MechRangers/Gameplay/Mechs/BaseLimb.h"

#include "GameBaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AGameBaseHUD : public AHUD
{
	GENERATED_BODY()


public:

	/** Crosshair asset pointer */
	UPROPERTY()
	UTexture2D* CrosshairTex;

	AGameBaseHUD();
	
	virtual void DrawHUD() override;

	/** Draw Crosshair on the screen */
	UFUNCTION(BlueprintCallable)
	void DrawCrosshair(FVector ScreenLocation, ELimbCrosshairType CrosshairType);

	/** Draw crosshair for each mech limb */
	UFUNCTION(BlueprintCallable)
	void DrawLimbsCrosshairs();
};
