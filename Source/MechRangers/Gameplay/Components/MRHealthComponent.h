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

	/** On Health State Changed Delegate */
	DECLARE_DELEGATE_TwoParams(FOnHealthStateChanged, UMRHealthComponent*, EHealthState);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	/** Delegate fires when Health State Was changed */
	FOnHealthStateChanged OnHealthStateChangedDelegate;

	// Sets default values for this component's properties
	UMRHealthComponent();
	
public:

	/** Max Health of an Element */
	UPROPERTY(Category=HealthComponent, EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	/** Current Health of an Element */
	UPROPERTY(Category=HealthComponent, EditAnywhere, BlueprintReadWrite)
	float CurrentHealth;

	/** When the Health state changes to the Damaged state. In percent from 0.f to 1.f of the CurrentHealth*/
	UPROPERTY(Category=HealthComponent, EditAnywhere, BlueprintReadWrite)
	float DamagedPercentage;

	/** Current Health State */
	UPROPERTY(Category=HealthComponent, EditAnywhere, BlueprintReadWrite)
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

	/** Get health percentage from 0.f to 1.f */
	UFUNCTION(BlueprintCallable)
    float GetHealthPercentage() const;

	/** Is the Health Component still Alive? */
	UFUNCTION(BlueprintCallable)
    bool Alive() const;

	/** Take damage to a Health Component. Return taken damage */
	UFUNCTION(BlueprintCallable)
    float TakeDamage(float Damage, FDamageTakenData const& DamageTakenData);
};
