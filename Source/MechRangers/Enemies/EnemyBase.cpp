// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "TimerManager.h"

#include "Log.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
	MaxHealth = 50.f;
	bAlive = true;
	TimeBeforeDestroy = FVector2D(3, 5);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	StartMovement();
}

bool AEnemyBase::GetMovePoint(AActor* ToActor, FVector& OutResult)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if(!NavSys)
		return false;
	 
	FNavLocation Result;

	bool const bSuccess = NavSys->GetRandomPointInNavigableRadius(ToActor->GetActorLocation(), 200.f, Result);

	//Out
	OutResult = Result.Location;
	return bSuccess;
}

void AEnemyBase::MoveToPoint(FVector& Point)
{
	AEnemyAIController* EnemyController = Cast<AEnemyAIController>(GetController());
	if (EnemyController)
	{

		EPathFollowingRequestResult::Type RequestResult = EnemyController->MoveToLocation(Point);

		if (RequestResult != EPathFollowingRequestResult::RequestSuccessful)
		{
			ULog::Error("Enemy Path Request Error!!!");
		} else
		{
			ULog::Success("Enemy Path Successfully Set!!!");
		}
			
	}
}


// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (bAlive)
	{
		TakeHealth(Damage);
	}

	return CurrentHealth;
}

void AEnemyBase::TakeHealth(const float Damage)
{
	CurrentHealth -= Damage;

	if (CurrentHealth <= 0)
	{
		Death();
	}
}

void AEnemyBase::Death()
{
	bAlive = false;
	
	AEnemyAIController* EnemyController = Cast<AEnemyAIController>(GetController());
	if (EnemyController)
	{
		EnemyController->StopMovement();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	float DestroyTime = FMath::RandRange(TimeBeforeDestroy.X, TimeBeforeDestroy.Y);

	// Destroy actor by timer 
	GetWorldTimerManager().SetTimer(TimerHandle_Destroy, this, &AEnemyBase::DestroyEnemy, DestroyTime, false);
}

void AEnemyBase::DestroyEnemy()
{
	Destroy();
}

void AEnemyBase::StartMovement()
{
	if (!MoveToActor)
		return;

	FVector MovePoint;
	if (GetMovePoint(MoveToActor, MovePoint))
	{
		MoveToPoint(MovePoint);
	}
	
}

void AEnemyBase::SetMoveToActor(AActor* NewActor)
{
	MoveToActor = NewActor;
}

bool AEnemyBase::IsAlive()
{
	return bAlive;
}


