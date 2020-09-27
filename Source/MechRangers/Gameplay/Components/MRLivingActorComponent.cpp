// Copyright PlatoSpace.com All Rights Reserved.

#include "MRLivingActorComponent.h"
#include "MRHealthComponent.h"

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
}

float UMRLivingActorComponent::TakeDamage(const float TakenDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ULog::Success("Object Takes Damage!!!");
	return TakenDamage;
}

float UMRLivingActorComponent::TakePointDamage(const float TakenDamage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UMRHealthComponent* DamageContainer = FindHealthContainer(FName("default"));
	if (DamageContainer)
	{
		DamageContainer->TakeDamage(TakenDamage);
		return TakenDamage;
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
