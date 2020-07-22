// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MechRangers/Gameplay/Weapons/WeaponBase.h"
#include "WeaponInstant.generated.h"

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
class MECHRANGERS_API AWeaponInstant : public AWeaponBase
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
    
};
