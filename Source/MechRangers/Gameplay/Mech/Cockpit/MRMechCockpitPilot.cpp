// Copyright PlatoSpace.com. All Rights Reserved.


#include "MRMechCockpitPilot.h"
#include "Components/SphereComponent.h"

// Sets default values
AMRMechCockpitPilot::AMRMechCockpitPilot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PilotAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PilotAttachmentPoint"));
	PilotAttachmentPoint->SetupAttachment(RootComponent);

	HeadZoneVisualizer = CreateDefaultSubobject<USphereComponent>(TEXT("HeadZoneVisualizer"));
	HeadZoneVisualizer->SetupAttachment(PilotAttachmentPoint);
	HeadZoneVisualizer->SetSphereRadius(10.f);
	HeadZoneVisualizer->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void AMRMechCockpitPilot::BeginPlay()
{
	Super::BeginPlay();
}

USceneComponent* AMRMechCockpitPilot::GetPilotAttachmentPoint() const
{
	return PilotAttachmentPoint;
}

USphereComponent* AMRMechCockpitPilot::GetHeadZoneVisualizer() const
{
	return HeadZoneVisualizer;
}
