// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/MRDisplayWidget.h"
#include "MRMechPaperDollWidget.generated.h"

class AMRMech;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRMechPaperDollWidget : public UMRDisplayWidget
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable)
    void SetOwnerMech(AMRMech* InMech);

protected:

    UPROPERTY(BlueprintReadWrite)
    AMRMech* OwnerMech;

    UFUNCTION(BlueprintImplementableEvent, Category=MechRangers)
    void NotifyOwnerMechSet();
};
