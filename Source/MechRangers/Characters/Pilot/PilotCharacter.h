// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "PilotCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API APilotCharacter : public AVRCharacter
{
	GENERATED_BODY()

public:
    void BeginPlay() override;
};
