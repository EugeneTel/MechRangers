// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MRMissionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMissionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// ctor
	UMRMissionComponent();

	// UActorComponent interface
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsInMission() const { return bIsInMission; }


protected:

	UPROPERTY(BlueprintReadWrite)
	bool bIsInMission;

		
};
