// Copyright PlatoSpace.com All Rights Reserved.

#include "CombatSystem/MRSimpleAimComponent.h"
#include "Log.h"
#include "DrawDebugHelpers.h"
#include "MechRangers/MechRangers.h"
#include "LivingSystem/MRDamageTakerInterface.h"

UMRSimpleAimComponent::UMRSimpleAimComponent()
{
	// Configs
	bDebug = false;
	bHitInform = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 0.1f;
	TraceEndPoint = FVector::ZeroVector;
}

// Called when the game starts
void UMRSimpleAimComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UMRSimpleAimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Trace();
}

void UMRSimpleAimComponent::Trace()
{
	const FVector StartPoint = GetComponentLocation();
	FVector EndPoint = StartPoint + GetForwardVector() * TraceLength;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bDebugQuery = true;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, COLLISION_WEAPON_TRACE, QueryParams))
	{
		EndPoint = HitResult.ImpactPoint;

		if (bHitInform)
		{	
			if (HitResult.Actor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
			{
				OnDamageTakerHitDelegate.ExecuteIfBound(HitResult);	
			}
		}
	}

	// Draw debug line
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, -1, 0.f, 2.f);

		DrawDebugSphere(GetWorld(), EndPoint, 32.f, 16, FColor::Red, false, 1, 0, 1);
	}

	// Save trace end point for future usage
	TraceEndPoint = EndPoint;
}

