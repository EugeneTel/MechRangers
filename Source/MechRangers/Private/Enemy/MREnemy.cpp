// Copyright PlatoSpace.com All Rights Reserved.

#include "Enemy/MREnemy.h"
#include "Log.h"
#include "DrawDebugHelpers.h"
#include "Enemy/MREnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "MechRangers/MechRangers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "CombatSystem/MRSimpleAimComponent.h"

// Sets default values
AMREnemy::AMREnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup components
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(RootComponent);
	AgroSphere->SetSphereRadius(600.f);
	AgroSphere->SetCollisionProfileName(FName("Trigger"));

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(RootComponent);
	CombatSphere->SetSphereRadius(90.f);
	CombatSphere->SetCollisionProfileName(FName("Trigger"));

	AimSystem = CreateDefaultSubobject<UMRSimpleAimComponent>(TEXT("AimSystem"));
	AimSystem->SetupAttachment(RootComponent);

	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	// Setup configs
	// TODO: update collision
	GetCapsuleComponent()->SetCollisionProfileName(FName("EnemyPawn"));
	AIControllerClass = AMREnemyAIController::StaticClass();
	MaxHealth = 50.f;
	bAlive = true;
	TimeBeforeDestroy = FVector2D(3, 5);
	MovementSpeedRange = FVector2D(500.f, 600.f);
	DamageRange = FVector2D(1.f, 2.f);
	GameplayTeam = EGameplayTeam::Alien;
	AgroChance = 0.5f;
	AttackType = EEnemyAttackType::EAT_Melee;
	TimeBetweenAttack = 0.5f;
}

// Called when the game starts or when spawned
void AMREnemy::BeginPlay()
{
	Super::BeginPlay();

	// Configs
	AIController = Cast<AMREnemyAIController>(GetController());
	AnimInstance = GetMesh()->GetAnimInstance();
	CurrentHealth = MaxHealth;
	AgroTarget = MainTarget;

	// Setup overlaps
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AMREnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AMREnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMREnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMREnemy::CombatSphereOnOverlapEnd);

	if (AttackType == EEnemyAttackType::EAT_Ranged)
	{
		AimSystem->SetComponentTickEnabled(true);
		InitWeapons();
	}

	// Setup movement
	UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (GetMovementComponent())
	{
		MovementComp->MaxWalkSpeed = FMath::RandRange(MovementSpeedRange.X, MovementSpeedRange.Y);
	}

	StartMovement();
}

// Called every frame
void AMREnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AMREnemy::GetMovePoint(AActor* ToActor, FVector& OutResult)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if(!NavSys)
		return false;
	 
	FNavLocation Result;

	bool const bSuccess = NavSys->GetRandomPointInNavigableRadius(ToActor->GetActorLocation(), 500.f, Result);

	//Out
	OutResult = Result.Location;
	return bSuccess;
}

void AMREnemy::OnTargetDeath(AActor* DeadActor)
{
	ClearActivities();
	SetMoveToActor(MainTarget);
	StartMovement();
}

void AMREnemy::MoveToPoint(FVector& Point)
{
	if (!AIController)
		return;

	MovementStatus = EEnemyMovementStatus::EMS_MoveToPoint;

	const EPathFollowingRequestResult::Type RequestResult = AIController->MoveToLocation(Point);

	if (RequestResult != EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy Path request error!"));
	}
}

void AMREnemy::ClearActivities()
{
	bAttacking = false;
	bOverlappingAgroSphere = false;
	bOverlappingCombatSphere = false;
	AgroTarget = MainTarget;
	CombatTarget = nullptr;

	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.5f);
	}

	if (AIController)
	{
		AIController->StopMovement();
	}

}

// Called to bind functionality to input
void AMREnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AMREnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bAlive)
	{
		TakeHealth(Damage);
	}

	return CurrentHealth;
}

EGameplayTeam AMREnemy::GetGameplayTeam() const
{
	return GameplayTeam;
}

float AMREnemy::GetAgroChance() const
{
	return AgroChance;
}

void AMREnemy::TakeHealth(const float Damage)
{
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0)
	{
		Death();
	}
}

void AMREnemy::Death()
{
	bAlive = false;
	MovementStatus = EEnemyMovementStatus::EMS_Dead;
	
	if (AIController)
	{
		AIController->StopMovement();
	}

	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.5);
	}

	if (OnDeathEvent.IsBound())
	{
		OnDeathEvent.Broadcast(this);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	const float DestroyTime = FMath::RandRange(TimeBeforeDestroy.X, TimeBeforeDestroy.Y);

	// Destroy actor by timer 
	GetWorldTimerManager().SetTimer(TimerHandle_Destroy, this, &AMREnemy::DestroyEnemy, DestroyTime, false);
}

void AMREnemy::DestroyEnemy()
{
	Destroy();
}

bool AMREnemy::IsAbleAttack(AActor* InActor) const
{
	if (!InActor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
		return false;

	IMRDamageTakerInterface* NewTarget = Cast<IMRDamageTakerInterface>(InActor);
	if (!NewTarget)
		return false;

	/** Only actors which implements interface and from different gameplay team */
	return NewTarget->Alive() && NewTarget->GetGameplayTeam() != GetGameplayTeam();
}

bool AMREnemy::ShouldAttack(const AActor* NewTarget, const AActor* CurrentTarget) const
{
	if (CurrentTarget == nullptr)
		return true;
	
	if (CurrentTarget != NewTarget && NewTarget->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
	{
		// Calculate random ability based on agro chance
		const float NewAgroChance = Cast<IMRDamageTakerInterface>(NewTarget)->GetAgroChance();
		const float Chance = FMath::RandRange(0.f, 1.f);
		return Chance <= NewAgroChance;
	}

	return false;
}

void AMREnemy::StartMovement()
{
	if (!MoveToActor)
		return;

	FVector MovePoint;
	if (GetMovePoint(MoveToActor, MovePoint))
	{
		MoveToPoint(MovePoint);
	}
	
}

void AMREnemy::MoveToTarget(AActor* Target)
{
	if (!AIController || !Alive())
		return;

	MovementStatus = EEnemyMovementStatus::EMS_MoveToTarget;

	FAIMoveRequest AIMoveRequest(Target);
	AIMoveRequest.SetAcceptanceRadius(1.f);

	FNavPathSharedPtr NavPathShared;

	AIController->MoveTo(AIMoveRequest, OUT &NavPathShared);

	/** Draw Debug line*/
	/*auto PathPoints = NavPathShared->GetPathPoints();

	FVector PreviousPoint = FVector(0, 0, 0);

	for (auto PathPoint : PathPoints)
	{
	auto EndPoint = PathPoint.Location;

	if (PreviousPoint.IsZero())
	{
	PreviousPoint = EndPoint;
	continue;
	}

	UKismetSystemLibrary::DrawDebugLine(GetWorld(), PreviousPoint, EndPoint, FLinearColor::Red, 5.f, 1.f);
	PreviousPoint = EndPoint;
	}*/
}

void AMREnemy::SetMainTarget(AActor* NewActor)
{
	MainTarget = NewActor;
}

void AMREnemy::SetMoveToActor(AActor* NewActor)
{
	MoveToActor = NewActor;
}

bool AMREnemy::Alive() const
{
	return bAlive;
}

IMRDamageTakerInterface::FOnDeath& AMREnemy::OnDeath()
{
	return OnDeathEvent;
}

void AMREnemy::AttackTarget(AActor* Target)
{
	if (!CombatMontage || !AnimInstance || !Alive())
		return;

	bAttacking = true;
	CombatTarget = Target;

	// Rotate actor to target
	FRotator LookAtTarget = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
	LookAtTarget.Pitch = GetActorRotation().Pitch;
	SetActorRotation(LookAtTarget);
	
	AnimInstance->Montage_Play(CombatMontage, 1.f);
}

void AMREnemy::AttackFinish()
{
	GetWorldTimerManager().SetTimer(TimerHandle_CheckAction, this, &AMREnemy::CheckAction, TimeBetweenAttack, true);
	RangedDamageEnd();
}

void AMREnemy::CheckAction()
{
	if (bOverlappingCombatSphere && CombatTarget)
	{
		AttackTarget(CombatTarget);
	} else if (bOverlappingAgroSphere && AgroTarget)
	{
		MoveToTarget(AgroTarget);
	} else
	{
		SetMoveToActor(MainTarget);
		StartMovement();
	}
}

void AMREnemy::MakeDamage()
{
	if (!Alive())
		return;
	
	if (AttackType == EEnemyAttackType::EAT_Melee)
	{
		MeleeDamageStart();
	} else if (AttackType == EEnemyAttackType::EAT_Ranged)
	{
		RangedDamageStart();
	}
}

void AMREnemy::MeleeDamageStart()
{
	const FVector StartTrace = GetActorLocation();
	const FVector EndTrace = StartTrace + GetActorForwardVector() * 300;

	const FHitResult& HitResult = AttackTrace(StartTrace, EndTrace);

	if (HitResult.bBlockingHit)
	{
		// @TODO: Check is target attackable and valid for attack
		
		AActor* AttackTarget = Cast<AActor>(HitResult.Actor);
		if (IsAbleAttack(AttackTarget))
		{
			UGameplayStatics::ApplyPointDamage(AttackTarget, 3, GetActorForwardVector(), HitResult, AIController, this, DamageTypeClass);
		}
	}	
}

void AMREnemy::InitWeapons()
{
	for (USceneComponent* const ChildScene : GetMesh()->GetAttachChildren())
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

void AMREnemy::RangedDamageStart()
{
	if (AIController)
	{
		AIController->StopMovement();
	}
	
	for (auto Weapon : Weapons)
	{
		if (IsValid(Weapon))
		{
			Weapon->StartFire();
		}
	}
}

void AMREnemy::RangedDamageEnd()
{
	for (auto Weapon : Weapons)
	{
		if (IsValid(Weapon))
		{
			Weapon->StopFire();
		}
	}
}

FHitResult AMREnemy::AttackTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, this);
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bDebugQuery = true;

	FHitResult Hit(ForceInit);
	//GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON_TRACE, TraceParams);

	// create a collision sphere
	FCollisionShape ColSphere = FCollisionShape::MakeSphere(80.0f);

	bool IsHit = GetWorld()->SweepSingleByChannel(Hit, TraceTo, TraceTo, FQuat::Identity, COLLISION_WEAPON_TRACE, ColSphere);

	// Draw debug line
	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceFrom, TraceTo, FColor::Green, false, 5, 0.f, 1.f);

		// draw collision sphere
		DrawDebugSphere(GetWorld(), TraceTo, ColSphere.GetSphereRadius(), 24, FColor::Purple, true);

		if (Hit.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 12.f, 8, FColor::Red,false,5.f,0,1.f);
		}
	}

	return Hit;
}

void AMREnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AMREnemy>(OtherActor) || !OtherActor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
		return;

	IMRDamageTakerInterface* NewTarget = Cast<IMRDamageTakerInterface>(OtherActor);
	if (!NewTarget)
		return;
	
	if (IsAbleAttack(OtherActor) && ShouldAttack(OtherActor, AgroTarget) && Alive())
	{
		bOverlappingAgroSphere = true;
		AgroTarget = OtherActor;

		// Subscribe on death
		NewTarget->OnDeath().AddUObject(this, &AMREnemy::OnTargetDeath);
		
		MoveToTarget(OtherActor);
	}
}

void AMREnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AgroTarget == OtherActor)
	{
		bOverlappingAgroSphere = false;
		AgroTarget = nullptr;
	}
}

void AMREnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AMREnemy>(OtherActor) || !OtherActor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
		return;

	IMRDamageTakerInterface* NewTarget = Cast<IMRDamageTakerInterface>(OtherActor);
	if (!NewTarget)
		return;
	
	if (IsAbleAttack(OtherActor) && Alive())
	{
		bOverlappingCombatSphere = true;
		AttackTarget(OtherActor);

		// Subscribe on death
		NewTarget->OnDeath().AddUObject(this, &AMREnemy::OnTargetDeath);
	}

}

void AMREnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CombatTarget == OtherActor)
	{
		bOverlappingCombatSphere = false;
		CombatTarget = nullptr;
	}
}


