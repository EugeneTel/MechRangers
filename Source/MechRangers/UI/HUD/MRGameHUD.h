// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MechRangers/Gameplay/Mechs/BaseLimb.h"

#include "MRGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AMRGameHUD : public AHUD
{
	GENERATED_BODY()

protected:

	/** Is VRMode */
	UPROPERTY(BlueprintReadWrite)
	bool bVRMode;

public:

	/** Crosshair asset pointer */
	UPROPERTY()
	UTexture2D* CrosshairTex;

	AMRGameHUD();

	void BeginPlay() override;
	
	virtual void DrawHUD() override;

	/** Draw Crosshair for Flat game mode on the screen */
	UFUNCTION(BlueprintCallable)
	void DrawCrosshairFlat(FVector CrosshairLocation, ELimbCrosshairType CrosshairType);

	/** Draw crosshair for each mech limb */
	UFUNCTION(BlueprintCallable)
	void DrawLimbsCrosshairs();
};
