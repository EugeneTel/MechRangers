// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MechRangersGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AMechRangersGameMode : public AGameMode
{
	GENERATED_BODY()

public:

    AMechRangersGameMode();

    /** Is VR Mode */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bVRMode;
};
