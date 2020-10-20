// Copyright PlatoSpace.com All Rights Reserved.


#include "MRHealthComponent.h"
#include "Log.h"

// Sets default values for this component's properties
UMRHealthComponent::UMRHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Setup Defaults
	DamagedPercentage = 0.5f;
	CurrentHealth = 100.f;
	MaxHealth = 100.f;
}

// Called when the game starts
void UMRHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMRHealthComponent::SetDamaged()
{
	SetHealthState(EHealthState::EHS_Damaged);
}

void UMRHealthComponent::SetDestroyed()
{
	SetHealthState(EHealthState::EHS_Destroyed);
}

void UMRHealthComponent::SetHealthState(const EHealthState NewHealthState)
{
	const EHealthState OldHealthState = HealthState;
	HealthState = NewHealthState;
	
	if (OnHealthStateChanged.IsBound())
	{
		const FHealthStateChangedParams HealthStateChangedParams {
			this,
			OldHealthState,
			HealthState
		};
		OnHealthStateChanged.Broadcast(HealthStateChangedParams);
	}
}

float UMRHealthComponent::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

bool UMRHealthComponent::Alive() const
{
	return (HealthState == EHealthState::EHS_Healthy ||  HealthState == EHealthState::EHS_Damaged);
}

float UMRHealthComponent::TakeDamage(const float Damage, FDamageTakenData const& DamageTakenData)
{
	float TakenDamage = 0.f;

	if (!Alive())
		return TakenDamage;

	// Calculate applied damage and current health
	const float OldHealth = CurrentHealth;
	CurrentHealth -= Damage;

	if (CurrentHealth < 0.f)
	{
		TakenDamage = Damage - FMath::Abs(CurrentHealth);
		CurrentHealth = 0.f;
	}
	else
	{
		TakenDamage = Damage;
	}

	// Set new health state if needed
	if (CurrentHealth == 0.f)
	{
		SetDestroyed();
	}
	else if (GetHealthPercentage() < DamagedPercentage && HealthState != EHealthState::EHS_Damaged)
	{
		SetDamaged();
	}
	else
	{
		// TODO: Implement event for Regular damage state
		// ULog::Number(TakenDamage, TEXT("Took damage: "), GetNameSafe(this));
		// ULog::Number(CurrentHealth, TEXT("Current health: "));
	}

	if (OnHealthChanged.IsBound())
	{
		const FHealthChangedParams HealthChangedParams = {
			this,
			OldHealth,
			CurrentHealth,
			MaxHealth,
			TakenDamage
		};
		
		OnHealthChanged.Broadcast(HealthChangedParams);
	}
	
	return TakenDamage;
}



