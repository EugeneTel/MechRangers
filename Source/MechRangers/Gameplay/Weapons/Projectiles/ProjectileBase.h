// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechRangers/Effects/ExplosionEffect.h"
#include "MechRangers/Gameplay/Weapons/WeaponProjectile.h"

#include "ProjectileBase.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS(Abstract, Blueprintable)
class MECHRANGERS_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();
	
	/** initial setup */
	virtual void PostInitializeComponents() override;

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection);

	/** handle hit */
	UFUNCTION()
    void OnImpact(const FHitResult& HitResult);

private:
	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UProjectileMovementComponent* MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UParticleSystemComponent* ParticleComp;

protected:

	/** effects for explosion */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<AExplosionEffect> ExplosionTemplate;

	/** controller that fired me (cache for damage calculations) */
	TWeakObjectPtr<AController> MyController;

	/** projectile data */
	struct FProjectileWeaponData WeaponConfig;

	/** did it explode? */
	UPROPERTY(Transient/*, ReplicatedUsing=OnRep_Exploded*/)
	bool bExploded;

	/** trigger explosion */
	void Explode(const FHitResult& Impact);

	/** shutdown projectile and prepare for destruction */
	void DisableAndDestroy();

	/** update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

protected:
	/** Returns MovementComp subobject **/
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }
	/** Returns CollisionComp subobject **/
	FORCEINLINE USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ParticleComp subobject **/
	FORCEINLINE UParticleSystemComponent* GetParticleComp() const { return ParticleComp; }

};
