// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMechHealthDisplay.h"
#include "Components/WidgetComponent.h"

// Sets default values
AMRMechHealthDisplay::AMRMechHealthDisplay()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisplayMesh->SetGenerateOverlapEvents(false);
	DisplayMesh->SetCollisionProfileName(FName("NoCollision"));
	RootComponent = DisplayMesh;

	DisplayWidget = CreateDefaultSubobject<UMRDisplayWidgetComponent>(TEXT("DisplayWidget"));
	DisplayWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisplayWidget->SetGenerateOverlapEvents(false);
	DisplayWidget->SetCollisionProfileName(FName("NoCollision"));
	DisplayWidget->SetupAttachment(RootComponent);
}

void AMRMechHealthDisplay::SetOwnerMech(AMRMech* InMech)
{
	OwnerMech = InMech;

	DisplayWidget->SetOwnerMech(InMech);
}