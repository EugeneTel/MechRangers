// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "MRMechCapsuleDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MECHRANGERS_API UMRMechCapsuleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
    
    /** Constructor */
    UMRMechCapsuleDataAsset();
    
    /** Half-height of the capsule */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capsule")
    float HalfHeight;

    /** Radius of the capsule */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capsule")
    float Radius;

    /** Offset from the center of the Mesh along the Z-axis */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capsule")
    float MeshOffset;

    /** Max Height the capsule can step up onto (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capsule|Movement")
    float MaxStepHeight;

    /** Max amount a slope (in degrees) the capsule can walk up */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capsule|Movement")
    float WalkableFloorAngle;
};
