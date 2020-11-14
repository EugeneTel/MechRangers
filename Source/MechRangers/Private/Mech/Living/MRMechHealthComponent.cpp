// Copyright PlatoSpace.com All Rights Reserved.

#include "Mech/Living/MRMechHealthComponent.h"
#include "Log.h"

// Sets default values for this component's properties
UMRMechHealthComponent::UMRMechHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Setup Defaults
	DamagedStateRatio = 0.3f;
	CurrentHealth = 20.f;
	MaxHealth = 20.f;
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

float UMRMechHealthComponent::GetHealthRatio() const
{
	return CurrentHealth / MaxHealth;
}

bool UMRMechHealthComponent::Alive() const
{
	return (HealthState == EPartHealthState::PHS_Healthy ||  HealthState == EPartHealthState::PHS_Damaged);
}

float UMRMechHealthComponent::TakeDamage(const float Value)
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
		Destroyed();
	}
	else if (GetHealthRatio() < DamagedStateRatio && HealthState != EPartHealthState::PHS_Damaged)
	{
		Damaged();
	}
	else
	{
		// TODO: Implement event for Regular damage state
		ULog::Number(TakenDamage, TEXT("Took damage: "), GetNameSafe(this));
		ULog::Number(CurrentHealth, TEXT("Current health: "));
	}
	
	return TakenDamage;
}

void UMRMechHealthComponent::Damaged()
{
	HealthState = EPartHealthState::PHS_Damaged;
	
	// TODO: Implement event for Damaged state
	ULog::Number(CurrentHealth, TEXT("Mech part Damaged. Health: "), GetNameSafe(this), LO_Both);
}

void UMRMechHealthComponent::Destroyed()
{
	HealthState = EPartHealthState::PHS_Destroyed;
	
	// TODO: Implement event for Destroyed state
	ULog::Number(CurrentHealth, TEXT("Mech part Destroyed. Health: "), GetNameSafe(this), LO_Both);
}

