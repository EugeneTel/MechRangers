// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MechRangers/Gameplay/Components/MRSimpleAimComponent.h"
#include "MechRangers/Gameplay/Interfaces/MRDamageTakerInterface.h"

#include "MRTurret.generated.h"

class AMRWeapon;
class UMRSimpleLivingActorComponent;
class UCapsuleComponent;

UCLASS(Abstract)
class MECHRANGERS_API AMRTurret : public APawn, public IMRDamageTakerInterface
{
	GENERATED_BODY()

protected:

	/** The CapsuleComponent being used for movement collision. */
	UPROPERTY(Category=Turret, VisibleAnywhere, BlueprintReadWrite);
	UCapsuleComponent* CapsuleComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this pawn's properties
	AMRTurret(const class FObjectInitializer& ObjectInitializer);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
//----------------------------------------------------------------------------------------------------------------------
// Configs
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Gameplay team (on which side a turret) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameplayTeam GameplayTeam;

	/**  A chance that attacker will attack a current object (if has other agro object). From 0.0 (will not attack) to 1.0 (attack) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AgroChance;

public:

	UFUNCTION(BlueprintCallable)
	virtual EGameplayTeam GetGameplayTeam() const override;

	UFUNCTION(BlueprintCallable)
	virtual float GetAgroChance() const override;

//----------------------------------------------------------------------------------------------------------------------
// Living system
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Delegate on death */
	FOnDeath OnDeathEvent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRSimpleLivingActorComponent* LivingActorComponent;
	
public:

	/** Turret takes damage */
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Subscribed to OnActorChangeHealthState in LivingActorComponent */
	void OnLivingActorChangeHealthState(AActor* InActor, EHealthState NewHealthState);

	/** Is the Turret Alive? */
	virtual bool Alive() const override;

	/** Turret damaged state implementation */
	UFUNCTION(BlueprintCallable)
	virtual void Damaged();

	/** Turret Death implementation */
	virtual void Destroyed();

	/** Interface implementation */
	virtual FOnDeath& OnDeath() override;
	
//----------------------------------------------------------------------------------------------------------------------
// Weapons
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Place where weapons should be attached */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* WeaponsAttachmentPoint;
	
	/** Installed weapons list */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AMRWeapon*> Weapons;

	/** Try to find all weapons attached to the Turret Pitch and save it to the weapons array for future use */
	void InitWeapons();

	/** Destroy all init weapons */
	void DestroyWeapons();

public:

	/** Get installed weapons */
	UFUNCTION(BlueprintCallable)
	TArray<AMRWeapon*>& GetWeapons();

//----------------------------------------------------------------------------------------------------------------------
// Combat
//----------------------------------------------------------------------------------------------------------------------
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
	class AActor* CombatTarget;

	bool IsAbleAttack(AActor* InActor) const;
	
	void AttackTarget(AActor* Target);
	
	void AttackFinish();
	
	void OnTargetDeath(AActor* DeadActor);

	/** Rotate turret components to Enemy direction */
	virtual void FollowCombatTarget(float DeltaTime) PURE_VIRTUAL(AMRTurret::FollowCombatTarget,);

	/** Try to find a target in combat zone for attack */ 
	AActor* FindTarget() const;

	virtual void StartWeaponFire();
	
	virtual void StopWeaponFire();

//----------------------------------------------------------------------------------------------------------------------
// Aiming
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Turret Aiming system */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMRSimpleAimComponent* AimSystem;

	/** Subscribed on AimSystem delegate */
	void OnAimSystemDamageTakerHit(FHitResult& HitResult);
};
