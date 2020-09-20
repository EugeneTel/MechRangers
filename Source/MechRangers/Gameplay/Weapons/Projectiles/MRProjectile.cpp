// Copyright PlatoSpace.com All Rights Reserved.

#include "MRProjectile.h"
#include "MechRangers/MechRangers.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


AMRProjectile::AMRProjectile()
{
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->AlwaysLoadOnClient = true;
    CollisionComp->AlwaysLoadOnServer = true;
    CollisionComp->bTraceComplexOnMove = true;
    CollisionComp->SetCollisionProfileName(FName("Projectile"));
    // CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    // CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
    // CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    // CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    // CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    // CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    RootComponent = CollisionComp;

    ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
    ParticleComp->bAutoActivate = false;
    ParticleComp->bAutoDestroy = false;
    ParticleComp->SetupAttachment(RootComponent);

    MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    MovementComp->UpdatedComponent = CollisionComp;
    MovementComp->InitialSpeed = 2000.0f;
    MovementComp->MaxSpeed = 2000.0f;
    MovementComp->bRotationFollowsVelocity = true;
    MovementComp->ProjectileGravityScale = 0.f;

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;
    SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
    bReplicates = true;
    SetReplicatingMovement(true);
}

void AMRProjectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    MovementComp->OnProjectileStop.AddDynamic(this, &AMRProjectile::OnImpact);
    CollisionComp->MoveIgnoreActors.Add(GetInstigator());

    AMRWeaponProjectile* OwnerWeapon = Cast<AMRWeaponProjectile>(GetOwner());
    if (OwnerWeapon)
    {
        OwnerWeapon->ApplyWeaponConfig(WeaponConfig);
    }

    SetLifeSpan( WeaponConfig.ProjectileLife );
    MyController = GetInstigatorController();
}

void AMRProjectile::InitVelocity(FVector& ShootDirection)
{
    if (MovementComp)
    {
        MovementComp->Velocity = ShootDirection * MovementComp->InitialSpeed;
    }
}

void AMRProjectile::OnImpact(const FHitResult& HitResult)
{
    if (GetLocalRole() == ROLE_Authority && !bExploded)
    {
        Explode(HitResult);
        DisableAndDestroy();
    }
}

void AMRProjectile::Explode(const FHitResult& Impact)
{
    if (ParticleComp)
    {
        ParticleComp->Deactivate();
    }

    // effects and damage origin shouldn't be placed inside mesh at impact point
    const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;

    if (WeaponConfig.ExplosionDamage > 0 && WeaponConfig.ExplosionRadius > 0 && WeaponConfig.DamageType)
    {
        UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, NudgedImpactLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get());
    }

    if (ExplosionTemplate)
    {
        FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
        AMRExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AMRExplosionEffect>(ExplosionTemplate, SpawnTransform);
        if (EffectActor)
        {
            EffectActor->SurfaceHit = Impact;
            UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
        }
    }

    bExploded = true;
}

void AMRProjectile::DisableAndDestroy()
{
    UAudioComponent* ProjAudioComp = FindComponentByClass<UAudioComponent>();
    if (ProjAudioComp && ProjAudioComp->IsPlaying())
    {
        ProjAudioComp->FadeOut(0.1f, 0.f);
    }

    MovementComp->StopMovementImmediately();

    // give clients some time to show explosion
    SetLifeSpan( 2.0f );
}

void AMRProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
    if (MovementComp)
    {
        MovementComp->Velocity = NewVelocity;
    }
}
