// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/WidgetComponent.h"
#include "MRDisplayWidgetComponent.generated.h"

class AMRMech;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API UMRDisplayWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SetOwnerMech(AMRMech* InMech);

protected:

	UPROPERTY(BlueprintReadWrite)
	AMRMech* OwnerMech;
	
};
