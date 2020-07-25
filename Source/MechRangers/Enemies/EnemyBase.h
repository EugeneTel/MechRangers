// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class MECHRANGERS_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Is Enemy Alive */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	bool bAlive;

	/** Max Health for Enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	/** Enemy current health */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	float CurrentHealth;

	/** Move enemy to selected actor */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	AActor* MoveToActor;

	/** Take Enemy Health and check death */
	UFUNCTION(BlueprintCallable)
	void TakeHealth(float Damage);

	/** Enemy Death implementation */
	UFUNCTION(BlueprintCallable)
	void Death();

	/** Destroy Enemy actor */
	UFUNCTION(BlueprintCallable)
	void DestroyEnemy();

	/** Start enemy movement */
	UFUNCTION(BlueprintCallable)
	void StartMovement();
	
	/** Move enemy to a Point */
    UFUNCTION(BlueprintCallable)
    void MoveToPoint(FVector& Point);

    /** Get point for enemy movement */
    UFUNCTION(BlueprintCallable)
    bool GetMovePoint(AActor* ToActor, FVector& OutResult);

	/** Min and Max time when actor will be destroyed after Enemy Death */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TimeBeforeDestroy;

	/** Timer for destroying enemy */
	FTimerHandle TimerHandle_Destroy;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
    AActor* DamageCauser) override;

	/** Set actor where enemy should move */
	UFUNCTION(BlueprintCallable)
	void SetMoveToActor(AActor* NewActor);

	/** Is Enemy IsAlive */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool IsAlive();

};
