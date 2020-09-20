// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

public:

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxArmor() const { return MaxArmor; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentArmor() const { return CurrentArmor; }

//----------------------------------------------------------------------------------------------------------------------
// Logic
//----------------------------------------------------------------------------------------------------------------------
public:

	/** Take damage to a Mech's part */
	UFUNCTION(BlueprintCallable)
	void TakeDamage(float Value);
};
