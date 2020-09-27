// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MechRangers/MechRangersTypes.h"
#include "MRHealthComponent.generated.h"




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRHealthComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// Sets default values for this component's properties
	UMRHealthComponent();
	
protected:

	/** Max Health of an Element */
	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth;

	/** Current Health of an Element */
	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float CurrentHealth;

	/** When the Health state changes to Damaged. In percent from 0.f to 1.f */
	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	float DamagedStateRatio;

	/** Current Health State */
	UPROPERTY(Category=Stats, EditDefaultsOnly, BlueprintReadWrite)
	EHealthState HealthState;

	/** Set Damaged state and all related functionality to this state */
	UFUNCTION(BlueprintCallable)
    virtual void SetDamaged();

	/** Set Destroyed state and all related functionality to this state */
	UFUNCTION(BlueprintCallable)
    virtual void SetDestroyed();

public:

	UFUNCTION(BlueprintCallable)
    FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
    FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable)
    FORCEINLINE EHealthState GetHealthState() const { return HealthState; }

	/** Get current health ratio from 0.f to 1.f */
	UFUNCTION(BlueprintCallable)
    float GetHealthRatio() const;

	/** Is the Health Component still Alive? */
	UFUNCTION(BlueprintCallable)
    bool Alive() const;

	/** Take damage to a Health Component. Return taken damage */
	UFUNCTION(BlueprintCallable)
    float TakeDamage(float Value);
};
