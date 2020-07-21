// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"

#include "PilotCharacter.generated.h"

class ABaseMech;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API APilotCharacter : public AVRCharacter
{
	GENERATED_BODY()

protected:
    
    /** Is in VR Mode */
    UPROPERTY(BlueprintReadWrite)
    bool bVRMode;

    /** Camera for Flat mode */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UCameraComponent* FlatCamera;

    /** A Mech owned by current pilot */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ABaseMech* OwningMech;

public:
    APilotCharacter();
    
    void BeginPlay() override;

    /** Is OwnedPilot in VR Mode */
    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsVRMode() const { return bVRMode; }

    /** Returns a Mech owned by current OwnedPilot */
    UFUNCTION(BlueprintCallable)
    FORCEINLINE ABaseMech* GetOwningMech() const { return OwningMech; };

    
};
