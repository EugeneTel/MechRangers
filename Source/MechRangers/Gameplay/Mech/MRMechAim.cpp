// Copyright PlatoSpace.com All Rights Reserved.


#include "MRMechAim.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "MechRangers/UI/HUD/MRCrosshair.h"
#include "MRMech.h"
#include "Cockpit/MRMechCockpit.h"

// Sets default values
AMRMechAim::AMRMechAim()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CrosshairType = ECrosshairType::ECT_None;
	bDebug = true;
}

// Called when the game starts or when spawned
void AMRMechAim::BeginPlay()
{
	Super::BeginPlay();

	// Cache mech
	OwnerMech = Cast<AMRMech>(GetOwner());

	if (CrosshairType != ECrosshairType::ECT_None && OwnerMech->HasPilot())
	{
		SpawnCrosshair();
	}
}

// Called every frame
void AMRMechAim::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Trace();
}

void AMRMechAim::Trace()
{
	const float TraceLength = 10000.f;
	const FVector StartPoint = GetActorLocation();
	FVector EndPoint = StartPoint + GetActorForwardVector() * TraceLength;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bDebugQuery = true;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		EndPoint = HitResult.ImpactPoint;
	}

	// Draw debug line
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, -1, 0.f, 2.f);
	}

	// Save trace end point for future usage
	TraceEndPoint = EndPoint;

	// Update Crosshair Position
	if (Crosshair)
	{
		Crosshair->SetActorLocation(EndPoint);

		// Rotate crosshair to the Pilot
		Crosshair->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Crosshair->GetActorLocation(), OwnerMech->Cockpit->GetActorLocation()));
	}
}

void AMRMechAim::SpawnCrosshair()
{
	if (!CrosshairClass)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.Owner = GetOwner();
	Crosshair = GetWorld()->SpawnActor<AMRCrosshair>(CrosshairClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	Crosshair->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

