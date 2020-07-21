// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLimb.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "MechRangers/UI/HUD/CrosshairBase.h"
#include "MechRangers/Gameplay/Characters/Pilot/PilotCharacter.h"
#include "MechRangers/Gameplay/Mechs/BaseMech.h"
#include "Log.h"

// Sets default values
ABaseLimb::ABaseLimb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create Components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// Setup Defaults
	ControlSpeed = 20.f;
	CrosshairType = ELimbCrosshairType::ELCT_None;
}

// Called when the game starts or when spawned
void ABaseLimb::BeginPlay()
{
	Super::BeginPlay();

	SpawnCrosshair();
}

// Called every frame
void ABaseLimb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Trace();
}

void ABaseLimb::Control(const float AxisX, const float AxisY)
{
	const float RotationSpeed = ControlSpeed * GetWorld()->GetDeltaSeconds(); 
	AddActorLocalRotation(FRotator(AxisY * RotationSpeed, AxisX * RotationSpeed, 0.f));
}

void ABaseLimb::Trace()
{
	const float TraceLength = 10000.f;
	const FVector StartPoint = Mesh->GetSocketLocation(FName("Muzzle"));
	FVector EndPoint = StartPoint + Mesh->GetForwardVector() * TraceLength;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bDebugQuery = true;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		EndPoint = HitResult.ImpactPoint;
	}

	DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, -1, 0.f, 2.f);

	// Save trace end point for future usage
	TraceEndPoint = EndPoint;

	// Update Crosshair Position
	if (Crosshair)
	{
		Crosshair->SetActorLocation(EndPoint);

		ABaseMech* Mech = Cast<ABaseMech>(GetOwner());
		if (Mech)
		{
			Crosshair->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Crosshair->GetActorLocation(), Mech->GetOwnedPilot()->GetActorLocation()));
		}
	}
}

void ABaseLimb::SpawnCrosshair()
{
	if (!CrosshairClass)
		return;

	FVector SpawnLocation = GetActorLocation();
	FActorSpawnParameters SpawnParams;
	Crosshair = GetWorld()->SpawnActor<ACrosshairBase>(CrosshairClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
}

