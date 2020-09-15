// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MechRangers/Gameplay/Weapons/MRWeapon.h"
#include "MRWeaponInstant.generated.h"

class AWeaponImpactEffect;

/**
 * Instant Weapon Data Structure
 */
USTRUCT()
struct FInstantWeaponData
{
    GENERATED_USTRUCT_BODY()

    /** base weapon spread (degrees) */
    UPROPERTY(EditDefaultsOnly, Category=Accuracy)
    float WeaponSpread;

    /** targeting spread modifier */
    UPROPERTY(EditDefaultsOnly, Category=Accuracy)
    float TargetingSpreadMod;

    /** continuous firing: spread increment */
    UPROPERTY(EditDefaultsOnly, Category=Accuracy)
    float FiringSpreadIncrement;

    /** continuous firing: max increment */
    UPROPERTY(EditDefaultsOnly, Category=Accuracy)
    float FiringSpreadMax;

    /** weapon range */
    UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
    float WeaponRange;

    /** damage amount */
    UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
    int32 HitDamage;

    /** type of damage */
    UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
    TSubclassOf<UDamageType> DamageType;

    /** hit verification: scale for bounding box of hit actor */
    UPROPERTY(EditDefaultsOnly, Category=HitVerification)
    float ClientSideHitLeeway;

    /** hit verification: threshold for dot product between view direction and hit direction */
    UPROPERTY(EditDefaultsOnly, Category=HitVerification)
    float AllowedViewDotHitDir;

    /** defaults */
    FInstantWeaponData()
    {
        WeaponSpread = 5.0f;
        TargetingSpreadMod = 0.25f;
        FiringSpreadIncrement = 1.0f;
        FiringSpreadMax = 10.0f;
        WeaponRange = 10000.0f;
        HitDamage = 10;
        DamageType = UDamageType::StaticClass();
        ClientSideHitLeeway = 200.0f;
        AllowedViewDotHitDir = 0.8f;
    }
};

/**
 * 
 */
UCLASS()
class MECHRANGERS_API AMRWeaponInstant : public AMRWeapon
{
	GENERATED_BODY()

protected:
    
    virtual EAmmoType GetAmmoType() const override
	{
	    return EAmmoType::EBullet;
	}

    /** weapon config */
    UPROPERTY(EditDefaultsOnly, Category=Config)
    FInstantWeaponData InstantConfig;

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------

protected:
    
    /** current spread from continuous firing */
    float CurrentFiringSpread;

    /** [local] weapon specific fire implementation */
    virtual void FireWeapon() override;

    /** [local + server] update spread on firing */
    virtual void OnBurstFinished() override;

    /** get current spread */
    float GetCurrentSpread() const;

    /** process the instant hit and notify the server if necessary */
    void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

    /** continue processing the instant hit, as if it has been confirmed by the server */
    void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

    /** check if weapon should deal damage to actor */
    bool ShouldDealDamage(AActor* TestActor) const;

    /** handle damage */
    void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

//----------------------------------------------------------------------------------------------------------------------
// Effects
//----------------------------------------------------------------------------------------------------------------------

protected:

    /** impact effects */
    UPROPERTY(EditDefaultsOnly, Category=Effects)
    TSubclassOf<AWeaponImpactEffect> ImpactTemplate;

    /** smoke trail */
    UPROPERTY(EditDefaultsOnly, Category=Effects)
    UParticleSystem* TrailFX;

    /** param name for beam target in smoke trail */
    UPROPERTY(EditDefaultsOnly, Category=Effects)
    FName TrailTargetParam;

    /** spawn effects for impact */
    void SpawnImpactEffects(const FHitResult& Impact);

    /** spawn trail effect */
    void SpawnTrailEffect(const FVector& EndPoint) const;
    
};
