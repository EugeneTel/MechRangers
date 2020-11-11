// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

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

	AMRGameHUD();
};
