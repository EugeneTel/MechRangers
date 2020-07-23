// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechRangers/MechRangersTypes.h"
#include "ExplosionEffect.generated.h"

class UPointLightComponent;
class USoundCue;

//
// Spawnable effect for explosion - NOT replicated to clients
// Each explosion type should be defined as separate blueprint
//
UCLASS(Abstract, Blueprintable)
class MECHRANGERS_API AExplosionEffect : public AActor
{
	GENERATED_BODY()

	/** explosion light */
	UPROPERTY(VisibleDefaultsOnly, Category=Effect)
	UPointLightComponent* ExplosionLight;
	
	/** Point light component name */
	FName ExplosionLightComponentName;

	/** explosion FX */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	UParticleSystem* ExplosionFX;

	/** Scale explosion FX */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	FVector ExplosionFXScale;

protected:
	/** spawn explosion */
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	AExplosionEffect(const FObjectInitializer& ObjectInitializer);

	/** Returns ExplosionLight subobject **/
	FORCEINLINE UPointLightComponent* GetExplosionLight() const { return ExplosionLight; }
	
	/** how long keep explosion light on? */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	float ExplosionLightFadeOut;

	/** explosion sound */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	USoundCue* ExplosionSound;
	
	/** explosion decals */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	struct FDecalData Decal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category=Surface)
	FHitResult SurfaceHit;

	/** update fading light */
	virtual void Tick(float DeltaSeconds) override;


};
