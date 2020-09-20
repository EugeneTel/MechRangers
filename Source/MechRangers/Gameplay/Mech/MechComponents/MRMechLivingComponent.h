// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MechRangers/Gameplay/Mech/MRMechTypes.h"

#include "MRMechLivingComponent.generated.h"

class UMRMechHealthComponent;

/**
 * Models the various health containing parts of a Mech, including interactions between health components
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMechLivingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Health Container for each mech part */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<EMechPart, UMRMechHealthComponent*> HealthContainers;

	/** Contains Bone names related to mech parts */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, EMechPart> BoneNameToMechPart;

	/** Find Damaged Health Container by Bone Name */
	UFUNCTION(BlueprintCallable)
    UMRMechHealthComponent* FindDamageHealthContainer(const FName BoneName);

public:
	// Sets default values for this component's properties
	UMRMechLivingComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Apply damage to Health Components */ 
	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float TakenDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	/** Apply Point damage to Health Components */
	UFUNCTION(BlueprintCallable)
	virtual float TakePointDamage(float TakenDamage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser);

	

		
};
