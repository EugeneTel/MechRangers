// Copyright PlatoSpace.com All Rights Reserved.

#include "CombatSystem/Effects/MRExplosionEffect.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PointLightComponent.h"

// Sets default values
AMRExplosionEffect::AMRExplosionEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ExplosionLightComponentName = TEXT("ExplosionLight");

	PrimaryActorTick.bCanEverTick = true;

	ExplosionLight = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, ExplosionLightComponentName);
	RootComponent = ExplosionLight;
	ExplosionLight->AttenuationRadius = 400.0;
	ExplosionLight->Intensity = 500.0f;
	ExplosionLight->bUseInverseSquaredFalloff = false;
	ExplosionLight->LightColor = FColor(255, 185, 35);
	ExplosionLight->CastShadows = false;
	ExplosionLight->SetVisibleFlag(true);

	ExplosionLightFadeOut = 0.2f;
	ExplosionFXScale = FVector::OneVector;
}

// Called when the game starts or when spawned
void AMRExplosionEffect::BeginPlay()
{
	Super::BeginPlay();

	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation(), ExplosionFXScale);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	if (Decal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(Decal.DecalMaterial, FVector(Decal.DecalSize, Decal.DecalSize, 1.0f),
            SurfaceHit.Component.Get(), SurfaceHit.BoneName,
            SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
            Decal.LifeSpan);
	}
	
}

// Called every frame
void AMRExplosionEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TimeAlive = GetWorld()->GetTimeSeconds() - CreationTime;
	const float TimeRemaining = FMath::Max(0.0f, ExplosionLightFadeOut - TimeAlive);

	if (TimeRemaining > 0)
	{
		const float FadeAlpha = 1.0f - FMath::Square(TimeRemaining / ExplosionLightFadeOut);

		UPointLightComponent* DefLight = Cast<UPointLightComponent>(GetClass()->GetDefaultSubobjectByName(ExplosionLightComponentName));
		ExplosionLight->SetIntensity(DefLight->Intensity * FadeAlpha);
	}
	else
	{
		Destroy();
	}

}

