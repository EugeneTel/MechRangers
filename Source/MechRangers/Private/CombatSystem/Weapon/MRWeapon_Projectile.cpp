// Copyright PlatoSpace.com All Rights Reserved.

#include "CombatSystem/Weapon/MRWeapon_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "CombatSystem/Projectile/MRProjectile.h"

void AMRWeapon_Projectile::ApplyWeaponConfig(FProjectileWeaponData& Data)
{
    Data = ProjectileConfig;
}

void AMRWeapon_Projectile::FireWeapon()
{
    FVector ShootDir = GetAdjustedAim();
    FVector Origin = GetMuzzleLocation();

    // trace from camera to check what's under crosshair
    const float ProjectileAdjustRange = 10000.0f;
    const FVector StartTrace = GetDamageStartLocation(ShootDir);
    const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
    FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

    // and adjust directions to hit that actor
    if (Impact.bBlockingHit)
    {
        const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
        bool bWeaponPenetration = false;

        const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
        if (DirectionDot < 0.0f)
        {
            // shooting backwards = weapon is penetrating
            bWeaponPenetration = true;
        }
        else if (DirectionDot < 0.5f)
        {
            // check for weapon penetration if angle difference is big enough
            // raycast along weapon mesh to check if there's blocking hit

            FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
            FVector MuzzleEndTrace = Origin;
            FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

            if (MuzzleImpact.bBlockingHit)
            {
                bWeaponPenetration = true;
            }
        }

        if (bWeaponPenetration)
        {
            // spawn at crosshair position
            Origin = Impact.ImpactPoint - ShootDir * 10.0f;
        }
        else
        {
            // adjust direction to hit
            ShootDir = AdjustedDir;
        }
    }

    ServerFireProjectile(Origin, ShootDir);
}

void AMRWeapon_Projectile::ServerFireProjectile_Implementation(const FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
    const FTransform SpawnTransform(ShootDir.Rotation(), Origin);
    AMRProjectile* Projectile = Cast<AMRProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTransform));
    if (Projectile)
    {
        Projectile->SetInstigator(GetInstigator());
        Projectile->SetOwner(this);
        Projectile->InitVelocity(ShootDir);

        UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
    }
}

bool AMRWeapon_Projectile::ServerFireProjectile_Validate(FVector Origin, FVector_NetQuantizeNormal ShootDir)
{
    return true;
}
