// Copyright PlatoSpace.com. All Rights Reserved.

#include "Mech/Cockpit/MRMechCockpit.h"
#include "Components/SphereComponent.h"

// Sets default values
AMRMechCockpit::AMRMechCockpit()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AMRMechCockpit::BeginPlay()
{
	Super::BeginPlay();

	// Cache Mech
	OwnerMech = Cast<AMRMech>(GetOwner());
}