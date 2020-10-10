// Copyright PlatoSpace.com All Rights Reserved.

#include "MRTurret.h"
#include "Log.h"
#include "MRTurretAIController.h"
#include "Components/SphereComponent.h"
#include "MechRangers/Gameplay/Interfaces/MRDamageTakerInterface.h"
#include "MechRangers/Gameplay/Components/MRSimpleLivingActorComponent.h"
#include "MechRangers/Gameplay/Weapons/MRWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMRTurret::AMRTurret(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = AMRTurretAIController::StaticClass();
	
	// Setup components
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DefaultSceneRoot"));
	CapsuleComponent->InitCapsuleSize(60.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(FName("EnemyPawn"));
	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(false);
	CapsuleComponent->SetCanEverAffectNavigation(true);
	CapsuleComponent->bDynamicObstacle = false;
	RootComponent = CapsuleComponent;

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(RootComponent);
	CombatSphere->SetSphereRadius(2000.f);
	CombatSphere->SetCollisionProfileName(FName("Trigger"));
	
	LivingActorComponent = CreateDefaultSubobject<UMRSimpleLivingActorComponent>(TEXT("LivingActorComponent"));

	// must be attached in children actors
	AimSystem = CreateDefaultSubobject<UMRSimpleAimComponent>(TEXT("AimSystem"));
	WeaponsAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponsAttachmentPoint"));

	// Set defaults
	GameplayTeam = EGameplayTeam::EGT_None;
	AgroChance = 0.5f;
	bAttacking = false;
	bFiring = false;
}

// Called when the game starts or when spawned
void AMRTurret::BeginPlay()
{
	Super::BeginPlay();

	// Delegates
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMRTurret::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMRTurret::CombatSphereOnOverlapEnd);
	LivingActorComponent->OnChangeHealthStateDelegate.BindUObject(this, &AMRTurret::OnLivingActorChangeHealthState);
	AimSystem->OnDamageTakerHitDelegate.BindUObject(this, &AMRTurret::OnAimSystemDamageTakerHit);
	
	InitWeapons();
}

// Called every frame
void AMRTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttacking && CombatTarget)
	{
		FollowCombatTarget(DeltaTime);
	}
}

void AMRTurret::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AMRTurret::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// we have already a target
	if (bAttacking && CombatTarget)
		return;
	
	if (Cast<AMRTurret>(OtherActor) || !OtherActor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
		return;

	IMRDamageTakerInterface* NewTarget = Cast<IMRDamageTakerInterface>(OtherActor);
	if (!NewTarget)
		return;
	
	if (IsAbleAttack(OtherActor) && Alive())
	{
		bOverlappingCombatSphere = true;
		AttackTarget(OtherActor);

		// Subscribe on death
		NewTarget->OnDeath().AddUObject(this, &AMRTurret::OnTargetDeath);
	}
}

void AMRTurret::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bAttacking = false;
	CombatTarget = nullptr;
	StopWeaponFire();
}

EGameplayTeam AMRTurret::GetGameplayTeam() const
{
	return GameplayTeam;
}

IMRDamageTakerInterface::FOnDeath& AMRTurret::OnDeath()
{
	return OnDeathEvent;
}

float AMRTurret::GetAgroChance() const
{
	return AgroChance;
}

float AMRTurret::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FDamageTakenData DamageTakenData = FDamageTakenData();
	DamageTakenData.DamageDealer = DamageCauser;

	// Try to attack damage instigator
	if (IsAbleAttack(EventInstigator->GetPawn()))
	{
		AttackTarget(EventInstigator->GetPawn());
	}

	return LivingActorComponent->TakeDamage(Damage, DamageTakenData);
}

void AMRTurret::OnLivingActorChangeHealthState(AActor* InActor, const EHealthState NewHealthState)
{
	if (NewHealthState == EHealthState::EHS_Damaged)
	{
		Damaged();
	} else if (NewHealthState == EHealthState::EHS_Destroyed)
	{
		Destroyed();
	}
}

bool AMRTurret::Alive() const
{
	return LivingActorComponent->Alive();
}

void AMRTurret::Damaged()
{
}

void AMRTurret::Destroyed()
{
	bAttacking = false;
	CombatTarget = nullptr;
	
	if (OnDeathEvent.IsBound())
	{
		OnDeathEvent.Broadcast(this);
	}

	DestroyWeapons();
	CombatSphere->DestroyComponent();
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void AMRTurret::InitWeapons()
{
	for (USceneComponent* const ChildScene : WeaponsAttachmentPoint->GetAttachChildren())
	{
		UChildActorComponent* ActorComponent = Cast<UChildActorComponent>(ChildScene);
		if (ActorComponent)
		{
			if (ActorComponent->GetChildActor())
			{
				AMRWeapon* ChildWeapon = Cast<AMRWeapon>(ActorComponent->GetChildActor());
				if (ChildWeapon)
				{
					ChildWeapon->SetAimSystem(AimSystem);
					ChildWeapon->SetEquipped(true);
					ChildWeapon->SetOwningPawn(this);
					Weapons.Add(ChildWeapon);
				}
			}
		}
	}
}

void AMRTurret::DestroyWeapons()
{
	for (auto Weapon : Weapons)
	{
		if (IsValid(Weapon))
		{
			Weapon->StopFire();
			Weapon->Destroy();
		}

	}
}

TArray<AMRWeapon*>& AMRTurret::GetWeapons()
{
	return Weapons;
}

bool AMRTurret::IsAbleAttack(AActor* InActor) const
{
	if (!InActor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
		return false;

	IMRDamageTakerInterface* NewTarget = Cast<IMRDamageTakerInterface>(InActor);
	if (!NewTarget)
		return false;

	/** Only actors which implements interface and from different gameplay team */
	return NewTarget->Alive() && NewTarget->GetGameplayTeam() != GetGameplayTeam();
}

void AMRTurret::AttackTarget(AActor* Target)
{
	if (!Alive())
		return;

	bAttacking = true;
	CombatTarget = Target;
	PrimaryActorTick.bCanEverTick = true;
}

void AMRTurret::AttackFinish()
{
	bAttacking = false;
	CombatTarget = nullptr;
	PrimaryActorTick.bCanEverTick = false;
	
	if (bOverlappingCombatSphere && CombatTarget)
	{
		AttackTarget(CombatTarget);
	}
}

void AMRTurret::OnTargetDeath(AActor* DeadActor)
{
	ULog::Success("Try to find another target!", LO_Both);

	StopWeaponFire();
	
	// Try to find another target
	AActor* NewTarget = FindTarget();
	if (NewTarget)
	{
		ULog::Success("Found another Target!", LO_Both);
		AttackTarget(NewTarget);
	}
}

AActor* AMRTurret::FindTarget() const
{
	TArray<AActor*> OverlappingActors;
	CombatSphere->GetOverlappingActors(OverlappingActors);
	if (OverlappingActors.Num() <= 0)
		return nullptr;
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor != CombatTarget && IsAbleAttack(OverlappingActor))
		{
			return OverlappingActor;
		}
	}

	return nullptr;
}

void AMRTurret::StartWeaponFire()
{
	if (bFiring || Weapons.Num() <= 0)
		return;

	bFiring = true;
	
	for (auto Weapon : Weapons)
	{
		if (IsValid(Weapon))
		{
			Weapon->StartFire();
		}
	}
}

void AMRTurret::StopWeaponFire()
{
	if (!bFiring || Weapons.Num() <= 0)
		return;

	bFiring = false;
	for (auto Weapon : Weapons)
	{
		if (IsValid(Weapon))
		{
			Weapon->StopFire();
		}

	}
}

void AMRTurret::OnAimSystemDamageTakerHit(FHitResult& HitResult)
{	
	if (IsAbleAttack(Cast<AActor>(HitResult.Actor)))
	{
		StartWeaponFire();

		// Look weapons at hit
		for (auto Weapon : Weapons)
		{
			if (IsValid(Weapon))
			{
				Weapon->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.ImpactPoint));
			}

		}
	}
}