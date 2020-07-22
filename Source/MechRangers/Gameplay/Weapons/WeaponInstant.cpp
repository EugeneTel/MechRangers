// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInstant.h"

void AWeaponInstant::FireWeapon()
{
    const int32 RandomSeed = FMath::Rand();
    const FRandomStream WeaponRandomStream(RandomSeed);
    const float CurrentSpread = GetCurrentSpread();
    const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

    const FVector AimDir = GetAdjustedAim();
    const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);

    const FVector StartTrace = GetDamageStartLocation(AimDir);
    const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

    const FHitResult Impact = WeaponTrace(StartTrace, EndTrace, true);

    // @TODO: Process instant hit (damage, effects etc.)

    CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement);
}

void AWeaponInstant::OnBurstFinished()
{
    Super::OnBurstFinished();

    CurrentFiringSpread = 0.0f;
}

float AWeaponInstant::GetCurrentSpread() const
{
    float FinalSpread = InstantConfig.WeaponSpread + CurrentFiringSpread;

    // @TODO: Targeting checking
    FinalSpread *= InstantConfig.TargetingSpreadMod;

    return FinalSpread;
}
