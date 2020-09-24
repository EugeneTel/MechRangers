// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MRPilotInterface.h"
#include "VRCharacter.h"
#include "MechRangers/MechRangersTypes.h"

#include "MRPilotVR.generated.h"

class AMRMech;
class UMRMechControlComponent;

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
class MECHRANGERS_API AMRPilotVR : public AVRCharacter, public IMRPilotInterface
{
	GENERATED_BODY()

protected:



public:
    AMRPilotVR();
    
    void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//----------------------------------------------------------------------------------------------------------------------
// Mech
//----------------------------------------------------------------------------------------------------------------------
protected:

    /** A Mech owned by current pilot */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AMRMech* Mech;

    UPROPERTY(Category=MRPilot, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
    UMRMechControlComponent* MechControl;

    /** Sit Pilot Into Mech. Inteface method implementation */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SitIntoMech(AMRMech* NewMech);

public:

    /** Returns a Mech owned by current OwnedPilot */
    UFUNCTION(BlueprintPure, BlueprintCallable)
    AMRMech* GetMech() const;

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------
protected:
    
    UFUNCTION(BlueprintCallable)
    void GripLeftPressed();

    UFUNCTION(BlueprintCallable)
    void GripRightPressed();

    UFUNCTION(BlueprintCallable)
    void GripLeftReleased();

    UFUNCTION(BlueprintCallable)
    void GripRightReleased();

    /** Input Mech Move Forward */
    void MechMoveForward(float Val);

    /** Input Mech Turn Rate */
    void MechTurnAtRate(float Val);

//----------------------------------------------------------------------------------------------------------------------
// Hands
//----------------------------------------------------------------------------------------------------------------------

protected:

    UPROPERTY(Category=Hands, EditDefaultsOnly, BlueprintReadWrite)
    class USphereComponent* GrabSphereRight;

    UPROPERTY(Category=Hands, EditDefaultsOnly, BlueprintReadWrite)
    class USphereComponent* GrabSphereLeft;

    UPROPERTY(Category=Hands, EditDefaultsOnly, BlueprintReadWrite)
    class USkeletalMeshComponent* HandMeshLeft;

    UPROPERTY(Category=Hands, EditDefaultsOnly, BlueprintReadWrite)
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
