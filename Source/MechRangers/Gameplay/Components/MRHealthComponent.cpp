// Copyright PlatoSpace.com All Rights Reserved.


#include "MRHealthComponent.h"
#include "Log.h"

// Sets default values for this component's properties
UMRHealthComponent::UMRHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Setup Defaults
	DamagedStateRatio = 0.3f;
	CurrentHealth = 20.f;
	MaxHealth = 20.f;
}

// Called when the game starts
void UMRHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMRHealthComponent::SetDamaged()
{
	HealthState = EHealthState::EHS_Damaged;
	
	// TODO: Implement event for Damaged state
	ULog::Number(CurrentHealth, TEXT("Object Damaged. Health: "), GetNameSafe(this), LO_Both);
}

void UMRHealthComponent::SetDestroyed()
{
	HealthState = EHealthState::EHS_Destroyed;
	
	// TODO: Implement event for Destroyed state
	ULog::Number(CurrentHealth, TEXT("Object Destroyed. Health: "), GetNameSafe(this), LO_Both);
}

float UMRHealthComponent::GetHealthRatio() const
{
	return CurrentHealth / MaxHealth;
}

bool UMRHealthComponent::Alive() const
{
	return (HealthState == EHealthState::EHS_Healthy ||  HealthState == EHealthState::EHS_Damaged);
}

float UMRHealthComponent::TakeDamage(const float Value)
{
	float TakenDamage = 0.f;

	if (!Alive())
		return TakenDamage;

	// Calculate applied damage and current health
	CurrentHealth -= Value;

	if (CurrentHealth < 0.f)
	{
		TakenDamage = Value - FMath::Abs(CurrentHealth);
		CurrentHealth = 0.f;
	}
	else
	{
		TakenDamage = Value;
	}

	// Set new health state if needed
	if (CurrentHealth == 0.f)
	{
		SetDestroyed();
	}
	else if (GetHealthRatio() < DamagedStateRatio && HealthState != EHealthState::EHS_Damaged)
	{
		SetDamaged();
	}
	else
	{
		// TODO: Implement event for Regular damage state
		ULog::Number(TakenDamage, TEXT("Took damage: "), GetNameSafe(this));
		ULog::Number(CurrentHealth, TEXT("Current health: "));
	}
	
	return TakenDamage;
}



