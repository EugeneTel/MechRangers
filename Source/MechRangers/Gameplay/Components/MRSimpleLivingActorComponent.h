// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MechRangers/MechRangersTypes.h"

#include "MRSimpleLivingActorComponent.generated.h"

class UMRHealthComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRSimpleLivingActorComponent : public UActorComponent
{
	GENERATED_BODY()

	/** Delegate Fires on Actor Death */
	DECLARE_DELEGATE_OneParam(FOnDeath, AActor*);
	
	/** Delegate Fires on Actor Changes Health State */
	DECLARE_DELEGATE_TwoParams(FOnChangeHealthState, AActor*, EHealthState);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Health container*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRHealthComponent* HealthContainer;

	/** Subscribed to the Health Component Health State Changed Delegate */
	UFUNCTION()
	void OnHealthContainerStateChanged(UMRHealthComponent* InHealthContainer, EHealthState InHealthState);

	/** Death implementation for the Living Actor and inform others */
	virtual void Death();

public:
	/** Actor Death delegate */
	FOnDeath OnDeathDelegate;
	
	/** Actor Change Health State */
	FOnChangeHealthState OnChangeHealthStateDelegate;
	
	// Sets default values for this component's properties
	UMRSimpleLivingActorComponent();
	
	/** Apply damage to Health Components */ 
	UFUNCTION(BlueprintCallable)
    virtual float TakeDamage(float TakenDamage, FDamageTakenData& DamageTakenData);

	/** Is Living Actor Component Alive */
	UFUNCTION(BlueprintCallable)
	bool Alive() const;

	/** Get current component Health State */
	UFUNCTION(BlueprintCallable)
	EHealthState GetHealthState();

	/** Kill an actor */
	UFUNCTION(BlueprintCallable)
	void Kill(FDamageTakenData& DamageTakenData);
};
