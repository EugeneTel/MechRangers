// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "MechRangers/MechRangersTypes.h"

#include "MRLivingActorComponent.generated.h"

class UMRHealthComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRLivingActorComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** List of available health containers */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, UMRHealthComponent*> HealthContainers;

	/** Find Health Container by parameter */
    virtual UMRHealthComponent* FindHealthContainer(const FName SearchName = FName("default"));

	/** Create all health containers. Must be executed in Constructor */
	virtual void CreateHealthContainers();

	/** Subscribe on all delegates for the component */
	virtual void SubscribeOnDelegates();

	/** Subscribed to the Health Component Health State Changed Delegate */
	UFUNCTION()
	void OnHealthContainerStateChanged(UMRHealthComponent* HealthContainer, EHealthState HealthState);

public:
	// Sets default values for this component's properties
	UMRLivingActorComponent();
	
	/** Apply damage to Health Components */ 
	UFUNCTION(BlueprintCallable)
    virtual float TakeDamage(float TakenDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	/** Apply Point damage to Health Components */
	//UFUNCTION(BlueprintCallable)
  //  virtual float TakePointDamage(float TakenDamage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser);

		
};
