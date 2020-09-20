// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMechHealthComponent.h"
#include "Log.h"

// Sets default values for this component's properties
UMRMechHealthComponent::UMRMechHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMRMechHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMRMechHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMRMechHealthComponent::TakeDamage(float Value)
{
	ULog::Success(TEXT("Takes damage mech part: "), LO_Both);
	ULog::Success(GetNameSafe(this), LO_Both);
	ULog::Success("-------------------------------", LO_Both);
}

