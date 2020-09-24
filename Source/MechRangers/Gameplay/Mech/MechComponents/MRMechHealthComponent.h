// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../MRMechTypes.h"
#include "MRMechHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMechHealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UMRMechHealthComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//----------------------------------------------------------------------------------------------------------------------
// Stats
//----------------------------------------------------------------------------------------------------------------------
protected:

	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float CurrentHealth;

	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float MaxArmor;

	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float CurrentArmor;

	/** When the Health state changes to Damaged. In percent from 0.f to 1.f */
	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float DamagedStateRatio;

	/** Current Health State */
	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	EPartHealthState HealthState;

public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxArmor() const { return MaxArmor; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentArmor() const { return CurrentArmor; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EPartHealthState GetHealthState() const { return HealthState; }

	/** Get current health ratio from 0.f to 1.f */
	UFUNCTION(BlueprintCallable)
	float GetHealthRatio() const;

	/** Is the Mech Part still Alive? */
	UFUNCTION(BlueprintCallable)
	bool Alive() const;

//----------------------------------------------------------------------------------------------------------------------
// Logic
//----------------------------------------------------------------------------------------------------------------------
public:

	/** Take damage to a Mech's part. Return taken damage */
	UFUNCTION(BlueprintCallable)
	float TakeDamage(float Value);

	/** Set Damaged state and all related functionality to this state */
	UFUNCTION(BlueprintCallable)
	void Damaged();

	/** Set Destroyed state and all related functionality to this state */
	UFUNCTION(BlueprintCallable)
	void Destroyed();
};
