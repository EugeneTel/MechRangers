// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MechRangersTypes.h"

#include "MRSingleHandAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRSingleHandAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gripping")
    EGripState GripState;
};
