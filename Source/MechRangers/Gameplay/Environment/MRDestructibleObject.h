// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechRangers/MechRangersTypes.h"
#include "MechRangers/Gameplay/Interfaces/MRDamageTakerInterface.h"

#include "MRDestructibleObject.generated.h"

class UMRSimpleLivingActorComponent;

UCLASS()
class MECHRANGERS_API AMRDestructibleObject : public AActor, public IMRDamageTakerInterface
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual float InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Apply damage to component (for example destructible component) */
	virtual float ApplyDamageToComponent(float Damage, FDamageTakenData const& DamageTakenData, TWeakObjectPtr<class UPrimitiveComponent> DamagedComponent);
	
public:	
	// Sets default values for this actor's properties
	AMRDestructibleObject();

//----------------------------------------------------------------------------------------------------------------------
// Configs
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Gameplay team (on which side an object) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameplayTeam GameplayTeam;

	/**  A chance that attacker will attack a current object (if has other agro object). From 0.0 (will not attack) to 1.0 (attack) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AgroChance;

public:
	virtual float GetAgroChance() const override;

	virtual EGameplayTeam GetGameplayTeam() const override;

//----------------------------------------------------------------------------------------------------------------------
// Living system
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Delegate on Enemy death */
	FOnDeath OnDeathEvent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRSimpleLivingActorComponent* LivingActorComponent;
	
public:

	/** Subscribed to OnLivingActorDeath Delegate */
	void OnLivingActorDeath(AActor* DeadActor);
	
	virtual bool Alive() const override;
	
	/** Delegate on death */
	virtual FOnDeath& OnDeath() override;
};
