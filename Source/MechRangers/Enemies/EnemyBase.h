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

	void MoveToPlayer();

	/** Max Health for Enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	/** Enemy current health */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	float CurrentHealth;

	/** Take Enemy Health and check death */
	UFUNCTION(BlueprintCallable)
	void TakeHealth(float Damage);

	/** Enemy Death implementation */
	UFUNCTION(BlueprintCallable)
	void Death();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
    AActor* DamageCauser) override;

};
