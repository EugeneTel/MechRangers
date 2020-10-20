// Copyright PlatoSpace.com All Rights Reserved.

#include "MRDestructibleObject.h"
#include "Log.h"
#include "MechRangers/Gameplay/Components/MRDestructiblePieceComponent.h"
#include "MechRangers/Gameplay/Components/MRSimpleLivingActorComponent.h"

// Sets default values
AMRDestructibleObject::AMRDestructibleObject()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	LivingActorComponent = CreateDefaultSubobject<UMRSimpleLivingActorComponent>(TEXT("LivingActorComponent"));

	// Set defaults
	GameplayTeam = EGameplayTeam::Neutral;
	AgroChance = 0.5f;
}

// Called when the game starts or when spawned
void AMRDestructibleObject::BeginPlay()
{
	Super::BeginPlay();

	LivingActorComponent->OnDeathDelegate.BindUObject(this, &AMRDestructibleObject::OnLivingActorDeath);
}

EGameplayTeam AMRDestructibleObject::GetGameplayTeam() const
{
	return GameplayTeam;
}

float AMRDestructibleObject::GetAgroChance() const
{
	return AgroChance;
}

void AMRDestructibleObject::OnLivingActorDeath(AActor* DeadActor)
{
	if (OnDeathEvent.IsBound())
	{
		OnDeathEvent.Broadcast(DeadActor);
	}
}

bool AMRDestructibleObject::Alive() const
{
	return LivingActorComponent->Alive();
}

IMRDamageTakerInterface::FOnDeath& AMRDestructibleObject::OnDeath()
{
	return OnDeathEvent;
}

float AMRDestructibleObject::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	for (auto HitResult : RadialDamageEvent.ComponentHits)
	{
		if (HitResult.Component.IsValid())
		{
			FDamageTakenData DamageTakenData = FDamageTakenData();
			DamageTakenData.DamageDealer = DamageCauser;

			ApplyDamageToComponent(Damage, DamageTakenData, HitResult.Component);

			return LivingActorComponent->TakeDamage(Damage, DamageTakenData);
		}
	}

	return 0.f;
}

float AMRDestructibleObject::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//ULog::Number(Damage, "InternalTakePointDamage ", PointDamageEvent.HitInfo.Component->GetOuter()->GetName(), LO_Both);
	
	if (PointDamageEvent.HitInfo.Component.IsValid())
	{
		FDamageTakenData DamageTakenData = FDamageTakenData();
		DamageTakenData.DamageDealer = DamageCauser;
		
		ApplyDamageToComponent(Damage, DamageTakenData, PointDamageEvent.HitInfo.Component);
		return LivingActorComponent->TakeDamage(Damage, DamageTakenData);
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

