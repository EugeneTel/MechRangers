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
	DECLARE_DELEGATE_OneParam(FOnLivingActorDeath, AActor*);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Health container*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRHealthComponent* HealthContainer;

	/** Subscribed to the Health Component Health State Changed Delegate */
	virtual void OnHealthContainerStateChanged(UMRHealthComponent* InHealthContainer, EHealthState InHealthState);

	/** Death implementation for the Living Actor and inform others */
	virtual void Death();

public:
	FOnLivingActorDeath OnLivingActorDeath;
	
	// Sets default values for this component's properties
	UMRSimpleLivingActorComponent();
	
	/** Apply damage to Health Components */ 
	UFUNCTION(BlueprintCallable)
    virtual float TakeDamage(float TakenDamage, FDamageTakenData& DamageTakenData);

	/** Is Living Actor Component Alive */
	UFUNCTION(BlueprintCallable)
	bool Alive() const;
};
