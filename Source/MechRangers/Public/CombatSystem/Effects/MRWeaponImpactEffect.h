// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechRangersTypes.h"
#include "MRWeaponImpactEffect.generated.h"

class USoundCue;

//
// Spawnable effect for weapon hit impact - NOT replicated to clients
// Each impact type should be defined as separate blueprint
//
UCLASS(Abstract, Blueprintable)
class MECHRANGERS_API AMRWeaponImpactEffect : public AActor
{
	GENERATED_BODY()
	
protected:

	/** get FX for material type */
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** get sound for material type */
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

public:
	/** default decal when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	struct FDecalData DefaultDecal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category=Surface)
	FHitResult SurfaceHit;

	AMRWeaponImpactEffect();

	/** spawn effect */
	virtual void PostInitializeComponents() override;

//----------------------------------------------------------------------------------------------------------------------
// FX
//----------------------------------------------------------------------------------------------------------------------
	/** default impact FX used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	UParticleSystem* DefaultFX;

//----------------------------------------------------------------------------------------------------------------------
// Sound
//----------------------------------------------------------------------------------------------------------------------
	/** default impact sound used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	USoundCue* DefaultSound;

	
};
