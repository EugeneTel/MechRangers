// Copyright PlatoSpace.com All Rights Reserved.

#include "LivingSystem/MRLivingActorComponent.h"
#include "LivingSystem/MRHealthComponent.h"

#include "Log.h"

UMRLivingActorComponent::UMRLivingActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	UMRLivingActorComponent::CreateHealthContainers();
}

// Called when the game starts
void UMRLivingActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe on Delegates
}

float UMRLivingActorComponent::TakeDamage(const float TakenDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UMRHealthComponent* DamageContainer = FindHealthContainer(FName("default"));
	if (DamageContainer)
	{
		//return DamageContainer->TakeDamage(TakenDamage);
	}

	return 0.f;
}

UMRHealthComponent* UMRLivingActorComponent::FindHealthContainer(const FName SearchName)
{
	if (HealthContainers.Contains(SearchName))
		return HealthContainers[SearchName];

	return nullptr;
}

void UMRLivingActorComponent::CreateHealthContainers()
{
	// create default HealthContainer
	HealthContainers.Add(FName("default"), CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthContainer")));	
}

void UMRLivingActorComponent::SubscribeOnDelegates()
{
	for (auto HealthContainer : HealthContainers)
	{
		//HealthContainer.Value->OnHealthStateChanged.AddDynamic(this, &UMRLivingActorComponent::OnHealthContainerStateChanged);
	}
}

void UMRLivingActorComponent::OnHealthContainerStateChanged(UMRHealthComponent* HealthContainer, EHealthState HealthState)
{
	ULog::Success("Health state changed!!!");
}
