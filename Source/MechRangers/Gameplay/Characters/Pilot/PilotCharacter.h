// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRCharacter.h"
#include "MechRangers/MechRangersTypes.h"

#include "PilotCharacter.generated.h"

class ABaseMech;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGripDropped, UGripMotionControllerComponent*, GripController, UObject*, DroppedObject);

UENUM(BlueprintType)
enum class EGripHand : uint8
{
    EGH_Left UMETA(DisplayName = "Left"),
    EGH_Right UMETA(DisplayName = "Right")
};

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

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Is OwnedPilot in VR Mode */
    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsVRMode() const { return bVRMode; }

    /** Returns a Mech owned by current OwnedPilot */
    UFUNCTION(BlueprintCallable)
    FORCEINLINE ABaseMech* GetOwningMech() const { return OwningMech; };

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------
    
    UFUNCTION(BlueprintCallable)
    void GripLeftPressed();

    UFUNCTION(BlueprintCallable)
    void GripRightPressed();

    UFUNCTION(BlueprintCallable)
    void GripLeftReleased();

    UFUNCTION(BlueprintCallable)
    void GripRightReleased();

//----------------------------------------------------------------------------------------------------------------------
// Hands
//----------------------------------------------------------------------------------------------------------------------

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
    class USphereComponent* GrabSphereRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
    class USphereComponent* GrabSphereLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
    class USkeletalMeshComponent* HandMeshLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hands")
    class USkeletalMeshComponent* HandMeshRight;

    UFUNCTION(BlueprintCallable)
    bool TraceFromController(UGripMotionControllerComponent* CallingController, FHitResult& OutHitResult) const;

    // Get Nearest overlapping object by tag
    UFUNCTION(BlueprintCallable)
    UPrimitiveComponent* GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent, FName Tag = "");

//----------------------------------------------------------------------------------------------------------------------
// Gripping
//----------------------------------------------------------------------------------------------------------------------

protected:

    UPROPERTY()
    FOnGripDropped OnGripDropped;
	
    UPROPERTY(BlueprintReadWrite)
    bool bGripPressedLeft;

    UPROPERTY(BlueprintReadWrite)
    bool bGripPressedRight;

    UPROPERTY(BlueprintReadWrite)
    EGripState GripStateLeft;

    UPROPERTY(BlueprintReadWrite)
    EGripState GripStateRight;

	
    UFUNCTION(BlueprintCallable)
    void CheckAndHandleGrip(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController);

    UFUNCTION(BlueprintCallable)
    void DropAllGrips(UGripMotionControllerComponent* CallingController) const;

    UFUNCTION()
    void GripDropped(UGripMotionControllerComponent* GripController, UObject* DroppedObject);

    UFUNCTION(BlueprintCallable)
    void CheckAndHandleGripAnimations();

    // Set Grip Controller Animation property
    UFUNCTION(BlueprintCallable)
    void CheckAndHandleGripControllerAnimations(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController, bool bGripPressed, EGripState& GripState);

    // Check is a Component valid for grip collision
    UFUNCTION(BlueprintCallable)
    static bool HasValidGripCollision(UPrimitiveComponent* Component);
};
