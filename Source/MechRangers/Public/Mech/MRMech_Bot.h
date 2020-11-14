// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mech/MRMech.h"
#include "MRMech_Bot.generated.h"

class AMRMechCockpit;

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AMRMech_Bot : public AMRMech
{
	GENERATED_BODY()

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	AMRMech_Bot(const class FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
    virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:

	/** A range where a Turret will attack a target */
	UPROPERTY(Category=Combat, EditDefaultsOnly, BlueprintReadWrite)
	class USphereComponent* CombatSphere;

	
	/** Is overlapping combat sphere? */
	UPROPERTY(Category=Combat, VisibleInstanceOnly, BlueprintReadWrite)
	bool bOverlappingCombatSphere;

	/** Is Attacking a Target */
	UPROPERTY(Category=Combat, VisibleInstanceOnly, BlueprintReadWrite)
	bool bAttacking;

	/** Is Turret firing */
	UPROPERTY(Category=Combat, VisibleInstanceOnly, BlueprintReadWrite)
	bool bFiring;

	/** Target for Attacking */
	UPROPERTY(Category=Combat, VisibleInstanceOnly, BlueprintReadWrite)
	AActor* CombatTarget;
	
	/** An actor was attacking a turret for the last time */
	UPROPERTY(BlueprintReadWrite)
	AActor* LastAttacker;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

	/** Handle for looking better target timer */
	FTimerHandle TimerHandle_HandleBetterTarget;

	bool IsAbleAttack(AActor* InActor) const;
	
	void AttackTarget(AActor* Target);
	
	void AttackFinish();

	void StopAttack();
	
	void OnTargetDeath(AActor* DeadActor);

	/** Try to find a target in combat zone for attack */ 
	AActor* FindTarget() const;

	/** Start firing from all weapons */
	virtual void StartWeaponFire();

	/** Stop firing all weapons */
	virtual void StopWeaponFire();

	/** Check should stop firing or not */
	virtual void HandleStopFiring();

	/** Check and attack better target if exist */
	virtual void HandleBetterTarget();

	/** Clear combat target (remove delegates etc.) */
	virtual void ClearCombatTarget();


	
};
