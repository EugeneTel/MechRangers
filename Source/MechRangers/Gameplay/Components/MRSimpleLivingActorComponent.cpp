// Copyright PlatoSpace.com All Rights Reserved.

#include "MRSimpleLivingActorComponent.h"
#include "MRHealthComponent.h"

#include "Log.h"

UMRSimpleLivingActorComponent::UMRSimpleLivingActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HealthContainer = CreateDefaultSubobject<UMRHealthComponent>(TEXT("LivingActorHealthContainer"));
}

// Called when the game starts
void UMRSimpleLivingActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe on Delegates
	HealthContainer->OnHealthStateChanged.BindUObject(this, &UMRSimpleLivingActorComponent::OnHealthContainerStateChanged);
}

float UMRSimpleLivingActorComponent::TakeDamage(const float TakenDamage, FDamageTakenData& DamageTakenData)
{
	return HealthContainer->TakeDamage(TakenDamage, DamageTakenData);
}

bool UMRSimpleLivingActorComponent::Alive() const
{
	return HealthContainer->Alive();
}

EHealthState UMRSimpleLivingActorComponent::GetHealthState()
{
	return HealthContainer->GetHealthState();
}

void UMRSimpleLivingActorComponent::Kill(FDamageTakenData& DamageTakenData)
{
	Death();
}

void UMRSimpleLivingActorComponent::OnHealthContainerStateChanged(UMRHealthComponent* InHealthContainer, EHealthState InHealthState)
{
	if (InHealthState == EHealthState::EHS_Destroyed)
	{
		Death();
	}

	OnChangeHealthStateDelegate.ExecuteIfBound(GetOwner(), InHealthState);	
}

void UMRSimpleLivingActorComponent::Death()
{
	OnDeathDelegate.ExecuteIfBound(GetOwner());	
}
