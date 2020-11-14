// Copyright PlatoSpace.com All Rights Reserved.

#include "CombatSystem/Weapons/MRWeaponInstant.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "CombatSystem/Effects/MRWeaponImpactEffect.h"

void AMRWeaponInstant::FireWeapon()
{
    const int32 RandomSeed = FMath::Rand();
    const FRandomStream WeaponRandomStream(RandomSeed);
    const float CurrentSpread = GetCurrentSpread();
    const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

    const FVector AimDir = GetAdjustedAim();
    const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);

    const FVector StartTrace = GetDamageStartLocation(AimDir);
    const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

    const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
    ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

    CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement);
}

void AMRWeaponInstant::OnBurstFinished()
{
    Super::OnBurstFinished();

    CurrentFiringSpread = 0.0f;
}

float AMRWeaponInstant::GetCurrentSpread() const
{
    float FinalSpread = InstantConfig.WeaponSpread + CurrentFiringSpread;

    // @TODO: Targeting checking
    FinalSpread *= InstantConfig.TargetingSpreadMod;

    return FinalSpread;
}

void AMRWeaponInstant::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir,
    int32 RandomSeed, float ReticleSpread)
{
    // @TODO: Implement Server Notification

    // process a confirmed hit
    ProcessInstantHit_Confirmed(Impact, Origin, ShootDir, RandomSeed, ReticleSpread);
}

void AMRWeaponInstant::ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin,
    const FVector& ShootDir, int32 RandomSeed, float ReticleSpread)
{
    // handle damage
    if (ShouldDealDamage(Impact.GetActor()))
    {
        DealDamage(Impact, ShootDir);
    }

    // @TODO: play FX on remote clients

    // play FX locally
    if (GetNetMode() != NM_DedicatedServer)
    {
        const FVector EndTrace = Origin + ShootDir * InstantConfig.WeaponRange;
        const FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

        SpawnTrailEffect(EndPoint);
        SpawnImpactEffects(Impact);
    }
}

bool AMRWeaponInstant::ShouldDealDamage(AActor* TestActor) const
{
    // if we're an actor on the server, or the actor's role is authoritative, we should register damage
    if (TestActor)
    {
        if (GetNetMode() != NM_Client ||
            TestActor->GetLocalRole() == ROLE_Authority ||
            TestActor->GetTearOff())
        {
            return true;
        }
    }

    return false;
}

void AMRWeaponInstant::DealDamage(const FHitResult& Impact, const FVector& ShootDir)
{
    FPointDamageEvent PointDmg;
    PointDmg.DamageTypeClass = InstantConfig.DamageType;
    PointDmg.HitInfo = Impact;
    PointDmg.ShotDirection = ShootDir;
    PointDmg.Damage = InstantConfig.HitDamage;   

    if (MyPawn)
    {
        AController* PawnController = MyPawn->GetController();

        if (PawnController)
        {
           Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, PawnController, MyPawn);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Effects
//----------------------------------------------------------------------------------------------------------------------

void AMRWeaponInstant::SpawnImpactEffects(const FHitResult& Impact)
{
    if (ImpactTemplate && Impact.bBlockingHit)
    {
        FHitResult UseImpact = Impact;

        // trace again to find component lost during replication
        if (!Impact.Component.IsValid())
        {
            const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
            const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
            FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
            UseImpact = Hit;
        }

        FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
        AMRWeaponImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AMRWeaponImpactEffect>(ImpactTemplate, SpawnTransform);
        if (EffectActor)
        {
            EffectActor->SurfaceHit = UseImpact;
            UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
        }
    }
}

void AMRWeaponInstant::SpawnTrailEffect(const FVector& EndPoint) const
{
    if (!TrailFX)
        return;
    
    const FVector Origin = GetMuzzleLocation();

    UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin, FRotator::ZeroRotator, FVector(3.f));
    if (TrailPSC)
    {
        TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
    }
}
