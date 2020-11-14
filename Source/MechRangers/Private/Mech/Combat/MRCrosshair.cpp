// Copyright PlatoSpace.com All Rights Reserved.

#include "Mech/Combat/MRCrosshair.h"

// Sets default values
AMRCrosshair::AMRCrosshair()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMRCrosshair::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMRCrosshair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

