// Copyright PlatoSpace.com All Rights Reserved.


#include "Mech/MRMechHitReactionComponent.h"

// Sets default values for this component's properties
UMRMechHitReactionComponent::UMRMechHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMRMechHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMRMechHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMRMechHitReactionComponent::PlayHit(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	
}

