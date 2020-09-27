// Copyright PlatoSpace.com All Rights Reserved.

#include "MRDestructibleObject.h"

// Sets default values
AMRDestructibleObject::AMRDestructibleObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMRDestructibleObject::BeginPlay()
{
	Super::BeginPlay();
	
}

