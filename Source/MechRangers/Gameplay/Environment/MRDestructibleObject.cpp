// Copyright PlatoSpace.com All Rights Reserved.

#include "MRDestructibleObject.h"
#include "../Components/MRLivingActorComponent.h"
#include "Log.h"
#include "MechRangers/Gameplay/Components/MRDestructiblePieceComponent.h"

// Sets default values
AMRDestructibleObject::AMRDestructibleObject()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

	// Create components
	// LivingComponent = CreateDefaultSubobject<UMRLivingActorComponent>(TEXT("LivingComponent"));
}

// Called when the game starts or when spawned
void AMRDestructibleObject::BeginPlay()
{
	Super::BeginPlay();
	
}

float AMRDestructibleObject::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ULog::Number(Damage, "InternalTakeRadialDamage ", "", LO_Both);
	
	for (auto HitResult : RadialDamageEvent.ComponentHits)
	{
		if (HitResult.Component.IsValid())
		{
			FDamageTakenData DamageTakenData = FDamageTakenData();
			DamageTakenData.DamageDealer = DamageCauser;

			ApplyDamageToComponent(Damage, DamageTakenData, HitResult.Component);
		}
	}

	return Damage;
}

float AMRDestructibleObject::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ULog::Number(Damage, "InternalTakePointDamage ", PointDamageEvent.HitInfo.Component->GetOuter()->GetName(), LO_Both);
	
	if (PointDamageEvent.HitInfo.Component.IsValid())
	{
		FDamageTakenData DamageTakenData = FDamageTakenData();
		DamageTakenData.DamageDealer = DamageCauser;
		
		return ApplyDamageToComponent(Damage, DamageTakenData, PointDamageEvent.HitInfo.Component);
	}
	


	return 0.f;
}

float AMRDestructibleObject::ApplyDamageToComponent(float Damage, FDamageTakenData const& DamageTakenData, TWeakObjectPtr<class UPrimitiveComponent> DamagedComponent)
{
	UMRDestructiblePieceComponent* DestructiblePiece = Cast<UMRDestructiblePieceComponent>(DamagedComponent->GetOuter());
	if (DestructiblePiece)
	{
		return DestructiblePiece->TakeDamage(Damage, DamageTakenData);
	}

	return Damage;
}

