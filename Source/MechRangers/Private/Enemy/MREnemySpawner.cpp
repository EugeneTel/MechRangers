// Copyright PlatoSpace.com All Rights Reserved.

#include "Enemy/MREnemySpawner.h"
#include "NavigationSystem.h"
#include "Enemy/MREnemy.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMREnemySpawner::AMREnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("RootComponent"));
	VisualComp = CreateDefaultSubobject<USphereComponent>(TEXT("VisualComp"));
	VisualComp->SetupAttachment(RootComponent);
	
	bSpawnerActive = false;
	Radius = 300.f;
	MaxEnemies = 10;
	SingleSpawnEnemiesRange = FIntPoint(2,4);
	SpawnIntervalRange = FVector2D(5, 10);
	SpawnScaleRange = FVector2D(1.f, 1.5f);
	
}

// Called when the game starts or when spawned
void AMREnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Spawn enemies
	if (bSpawnerActive)
	{
		SpawnEnemies();
	}
}

// Called every frame
void AMREnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMREnemySpawner::SpawnEnemies()
{
	ensure(EnemyClass);

	if (!bSpawnerActive)
		return;

	// Calculate how many to spawn
	int32 SpawnNumber = FMath::RandRange(SingleSpawnEnemiesRange.X, SingleSpawnEnemiesRange.Y);
	SpawnNumber = FMath::Min(SpawnNumber, MaxEnemies - SpawnedEnemies);
	if (SpawnNumber <= 0)
		return;

	// Spawn each enemy
	FVector SpawnLocation;
	for (int32 i = 0; i < SpawnNumber; ++i)
	{
		if (GetRandomPoint(SpawnLocation))
		{
			if (SpawnEnemy(SpawnLocation))
			{
				SpawnedEnemies++;
			}
		}
	}

	// Set timer for the next spawn
	const float NextSpawnTime = FMath::RandRange(SpawnIntervalRange.X, SpawnIntervalRange.Y);
	if (NextSpawnTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_NextSpawn, this, &AMREnemySpawner::SpawnEnemies, NextSpawnTime, false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawn Enemies Finished. Total number: %i"), SpawnedEnemies);
}

bool AMREnemySpawner::GetRandomPoint(FVector& OutResult)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if(!NavSys)
		return false;
	 
	FNavLocation Result;

	bool bSuccess = NavSys->GetRandomPointInNavigableRadius(GetActorLocation(), Radius, Result);

	//Out
	OutResult = Result.Location;
	return bSuccess;
}

bool AMREnemySpawner::SpawnEnemy(FVector& Location)
{
	if (!EnemyClass)
		return false;

	const FVector SpawnScale(FMath::RandRange(SpawnScaleRange.X, SpawnScaleRange.Y));
	const FTransform SpawnTransform(FRotator::ZeroRotator, Location, SpawnScale);

	AMREnemy* NewEnemy = Cast<AMREnemy>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, EnemyClass, SpawnTransform));
	if (NewEnemy)
	{
		NewEnemy->SetInstigator(GetInstigator());
		NewEnemy->SetOwner(this);
		NewEnemy->SetMoveToActor(EnemyMainTarget);
		NewEnemy->SetMainTarget(EnemyMainTarget);

		UGameplayStatics::FinishSpawningActor(NewEnemy, SpawnTransform);
	}
	else
	{
		return false;
	}

	return true;
}

void AMREnemySpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	VisualComp->SetSphereRadius(Radius);
}

void AMREnemySpawner::Activate()
{
	bSpawnerActive = true;
	SpawnEnemies();
}

void AMREnemySpawner::Deactivate()
{
	bSpawnerActive = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_NextSpawn);
}

void AMREnemySpawner::Reset()
{
	SpawnedEnemies = 0;
}

