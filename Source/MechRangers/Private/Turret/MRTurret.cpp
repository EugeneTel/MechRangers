// Copyright PlatoSpace.com All Rights Reserved.

#include "Turret/MRTurret.h"
#include "Log.h"
#include "Turret/MRTurretAIController.h"
#include "Components/SphereComponent.h"
#include "LivingSystem/MRDamageTakerInterface.h"
#include "LivingSystem/MRSimpleLivingActorComponent.h"
#include "CombatSystem/Weapons/MRWeapon.h"
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
	AimSystem->SetTraceLength(2000.f);
	
	WeaponsAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponsAttachmentPoint"));

	// Set defaults
	GameplayTeam = EGameplayTeam::Neutral;
	AgroChance = 0.5f;
	bAttacking = false;
	bFiring = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
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
	
	if (Cast<AMRTurret>(OtherActor))
		return;

	if (IsAbleAttack(OtherActor) && Alive())
	{
		bOverlappingCombatSphere = true;
		AttackTarget(OtherActor);
	}
}

void AMRTurret::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	StopAttack();
	ClearCombatTarget();

	// Try to find another target
	AActor* NewTarget = FindTarget();
	if (NewTarget)
	{
		AttackTarget(NewTarget);
	}
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

void AMRTurret::AimingActivate()
{
	SetActorTickEnabled(true);
	if (AimSystem)
	{
		AimSystem->SetComponentTickEnabled(true);
	}
}

void AMRTurret::AimingDeactivate()
{
	SetActorTickEnabled(false);
	if (AimSystem)
	{
		AimSystem->SetComponentTickEnabled(false);
	}
}

float AMRTurret::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	FDamageTakenData DamageTakenData = FDamageTakenData();
	DamageTakenData.DamageDealer = DamageCauser;

	// Try to attack damage instigator
	if (IsValid(EventInstigator) && IsAbleAttack(EventInstigator->GetPawn()))
	{
		LastAttacker = EventInstigator->GetPawn();
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
		Death();
	}
}

bool AMRTurret::Alive() const
{
	return LivingActorComponent->Alive();
}

void AMRTurret::Damaged()
{
}

void AMRTurret::Death()
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
	if (!Alive() || !IsValid(Target))
		return;

	if (Target->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
	{
		// Subscribe on death
		Cast<IMRDamageTakerInterface>(Target)->OnDeath().AddUObject(this, &AMRTurret::OnTargetDeath);
	}

	bAttacking = true;
	CombatTarget = Target;
	AimingActivate();

	// set timer for looking better target
	if (TimerHandle_HandleBetterTarget.IsValid() == false)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleBetterTarget, this, &AMRTurret::HandleBetterTarget, 3.f, true);
	}

}

void AMRTurret::AttackFinish()
{
	StopAttack();
	ClearCombatTarget();
	
	if (bOverlappingCombatSphere && CombatTarget)
	{
		AttackTarget(CombatTarget);
	}
}

void AMRTurret::StopAttack()
{
	bAttacking = false;
	//StopWeaponFire();
	AimingDeactivate();

	if (TimerHandle_HandleBetterTarget.IsValid())
	{
		TimerHandle_HandleBetterTarget.Invalidate();
	}
}

void AMRTurret::OnTargetDeath(AActor* DeadActor)
{
	// ULog::Success("Try to find another target!", LO_Both);

	//StopAttack();
	ClearCombatTarget();
	
	// Try to find another target
	AActor* NewTarget = FindTarget();
	if (NewTarget)
	{
		// ULog::Success("Found another Target!", LO_Both);
		AttackTarget(NewTarget);
	}
}

AActor* AMRTurret::FindTarget() const
{
	if (!Alive())
		return nullptr;
	
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

	// set timer for check stop firing
	GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AMRTurret::HandleStopFiring, 1.5f, true);
	
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

	if (TimerHandle_HandleFiring.IsValid())
	{
		TimerHandle_HandleFiring.Invalidate();
	}
}

void AMRTurret::HandleStopFiring()
{
	if (bFiring && !CombatTarget)
	{
		StopWeaponFire();
	}
}

void AMRTurret::HandleBetterTarget()
{
	// Try to attack damage causer first
	if (LastAttacker && IsValid(LastAttacker) && LastAttacker != CombatTarget)
	{
		AttackTarget(LastAttacker);
	}

	LastAttacker = nullptr;
}

void AMRTurret::ClearCombatTarget()
{
	if (IsValid(CombatTarget) && CombatTarget->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
	{
		Cast<IMRDamageTakerInterface>(CombatTarget)->OnDeath().RemoveAll(this);
	}
	CombatTarget = nullptr;
}

void AMRTurret::OnAimSystemDamageTakerHit(FHitResult& HitResult)
{	
	if (IsAbleAttack(Cast<AActor>(HitResult.Actor)))
	{
		StartWeaponFire();

		// Look weapons at hit
		// for (auto Weapon : Weapons)
		// {
		// 	if (IsValid(Weapon))
		// 	{
		// 		Weapon->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.ImpactPoint));
		// 	}
		//
		// }
	}
}
