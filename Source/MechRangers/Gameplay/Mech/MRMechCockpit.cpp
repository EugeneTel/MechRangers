// Copyright PlatoSpace.com. All Rights Reserved.


#include "MRMechCockpit.h"
#include "Components/SphereComponent.h"

// Sets default values
AMRMechCockpit::AMRMechCockpit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PilotAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PilotAttachmentPoint"));
	PilotAttachmentPoint->SetupAttachment(RootComponent);

	HeadZoneVisualizer = CreateDefaultSubobject<USphereComponent>(TEXT("HeadZoneVisualizer"));
	HeadZoneVisualizer->SetupAttachment(PilotAttachmentPoint);
	HeadZoneVisualizer->SetSphereRadius(10.f);
	HeadZoneVisualizer->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void AMRMechCockpit::BeginPlay()
{
	Super::BeginPlay();
	
}

USceneComponent* AMRMechCockpit::GetPilotAttachmentPoint() const
{
	return PilotAttachmentPoint;
}
