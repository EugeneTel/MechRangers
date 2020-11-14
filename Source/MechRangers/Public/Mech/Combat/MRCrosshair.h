// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MRCrosshair.generated.h"

UCLASS()
class MECHRANGERS_API AMRCrosshair : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AMRCrosshair();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
