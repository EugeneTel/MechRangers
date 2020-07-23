// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponImpactEffect.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

AWeaponImpactEffect::AWeaponImpactEffect()
{
    SetAutoDestroyWhenFinished(true);
}

UParticleSystem* AWeaponImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
    // @TODO: Implement for other surfaces

    UParticleSystem* ImpactFX = DefaultFX;

    return ImpactFX;
}

USoundCue* AWeaponImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
    // @TODO: Implement for other surfaces

    USoundCue* ImpactSound = DefaultSound;

    return ImpactSound;
}

void AWeaponImpactEffect::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
    const EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

    // show particles
    UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
    if (ImpactFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
    }

    // play sound
    USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
    }

    if (DefaultDecal.DecalMaterial)
    {
        FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
        RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

        UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(1.0f, DefaultDecal.DecalSize, DefaultDecal.DecalSize),
            SurfaceHit.Component.Get(), SurfaceHit.BoneName,
            SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
            DefaultDecal.LifeSpan);
    }
}
