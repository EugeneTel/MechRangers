// Copyright PlatoSpace.com All Rights Reserved.

#include "MRSimpleAimComponent.h"
#include "Log.h"
#include "DrawDebugHelpers.h"
#include "MechRangers/Gameplay/Interfaces/MRDamageTakerInterface.h"

// Sets default values for this component's properties
/*UMRSimpleAimComponent::UMRSimpleAimComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	SetActiveFlag(false);

#if WITH_EDITOR
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
#endif
}*/

UMRSimpleAimComponent::UMRSimpleAimComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
// #if WITH_EDITOR
// 	ArrowComponent = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("ArrowComponent"));
// 	ArrowComponent->SetupAttachment(GetOwner()->GetRootComponent());
// #endif

	// Configs
	bDebug = false;
}


// Called when the game starts
void UMRSimpleAimComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.bCanEverTick = false;
	SetActiveFlag(true);
	SetComponentTickEnabled(true);
	
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
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility, QueryParams))
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

