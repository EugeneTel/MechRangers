// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"

#include "Log.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
	MaxHealth = 50.f;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	//GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyBase::MoveToPlayer);

	MoveToPlayer();
	
}

void AEnemyBase::MoveToPlayer()
{
	AEnemyAIController* EnemyController = Cast<AEnemyAIController>(GetController());
	if (EnemyController)
	{
		auto PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			auto PlayerPawn = PC->GetPawn();
			if (PlayerPawn)
			{
				auto RequestResult = EnemyController->MoveToActor(PlayerPawn);
			}
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
	TakeHealth(Damage);

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
	Destroy();
}


