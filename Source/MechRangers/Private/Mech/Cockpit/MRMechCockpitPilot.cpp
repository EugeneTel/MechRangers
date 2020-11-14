// Copyright PlatoSpace.com. All Rights Reserved.


#include "Mech/Cockpit/MRMechCockpitPilot.h"
#include "Components/SphereComponent.h"
#include "Mech/Living/MRMechHealthDisplay.h"

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
	HeadZoneVisualizer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadZoneVisualizer->SetCollisionProfileName(FName("NoCollision"));

	HealthDisplayContainer = CreateDefaultSubobject<UChildActorComponent>(TEXT("HealthDisplay"));
	HealthDisplayContainer->SetupAttachment(RootComponent);
	
	MissionDisplayContainer = CreateDefaultSubobject<UChildActorComponent>(TEXT("MissionDisplay"));
	MissionDisplayContainer->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMRMechCockpitPilot::BeginPlay()
{
	Super::BeginPlay();

	SetupHealthDisplay();
}

USceneComponent* AMRMechCockpitPilot::GetPilotAttachmentPoint() const
{
	return PilotAttachmentPoint;
}

USphereComponent* AMRMechCockpitPilot::GetHeadZoneVisualizer() const
{
	return HeadZoneVisualizer;
}

void AMRMechCockpitPilot::SetupHealthDisplay() const
{
	AMRMechHealthDisplay* HealthDisplay = Cast<AMRMechHealthDisplay>(HealthDisplayContainer->GetChildActor());
	if (IsValid(HealthDisplay) && IsValid(OwnerMech))
	{
		HealthDisplay->SetOwnerMech(OwnerMech);
	}
}
