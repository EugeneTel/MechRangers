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

void UMRSimpleLivingActorComponent::OnHealthContainerStateChanged(UMRHealthComponent* InHealthContainer, EHealthState InHealthState)
{
	if (InHealthState == EHealthState::EHS_Destroyed)
	{
		Death();
	}
}

void UMRSimpleLivingActorComponent::Death()
{
	ULog::Success("UMRSimpleLivingActorComponent::Death");
	OnLivingActorDeath.ExecuteIfBound(GetOwner());	
}
