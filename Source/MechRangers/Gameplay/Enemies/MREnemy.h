// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MREnemyAIController.h"
#include "GameFramework/Character.h"
#include "MREnemy.generated.h"

class USphereComponent;

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle UMETA(DispayName = "Idle"),
    EMS_MoveToPoint UMETA(DisplayName = "MoveToPoint"),
    EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
    EMS_Attacking UMETA(DisplayName = "Attacking"),
    EMS_Dead UMETA(DisplayName = "Dead"),

    EMS_Invalid UMETA(DisplayName = "Invalid")
};

UCLASS()
class MECHRANGERS_API AMREnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMREnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	AMREnemyAIController* AIController;

	UPROPERTY(BlueprintReadOnly)
	UAnimInstance* AnimInstance;

	/** Set debug mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug;

	/** Is Enemy Alive */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	bool bAlive;

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

	/** Destroy Enemy actor */
	UFUNCTION(BlueprintCallable)
	void DestroyEnemy();

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

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Is Enemy IsAlive */
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool Alive();

//----------------------------------------------------------------------------------------------------------------------
// Movement
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Random between min and max movement speed */
	UPROPERTY(Category=Movement, EditAnywhere, BlueprintReadWrite)
	FVector2D MovementSpeedRange;

	/** Move enemy to selected actor */
	UPROPERTY(Category=Movement, VisibleInstanceOnly, BlueprintReadWrite)
	AActor* MoveToActor;

	/** A range where an Enemy will get a New target for following */  
	UPROPERTY(Category=Movement, EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* AgroSphere;

	/** Target for following */
	UPROPERTY(Category=Movement, VisibleInstanceOnly, BlueprintReadWrite)
	class AActor* AgroTarget;

	/** Is anybody attackable overriding an Agro Sphere */
	UPROPERTY(Category=Movement, VisibleInstanceOnly, BlueprintReadWrite)
	bool bOverlappingAgroSphere;
		
	/** Enemy movement status */
	UPROPERTY(Category=Movement, VisibleInstanceOnly, BlueprintReadWrite)
	EEnemyMovementStatus MovementStatus;

public:

	/** Set actor where enemy should move to */
	UFUNCTION(BlueprintCallable)
    void SetMoveToActor(AActor* NewActor);
	
	/** Start enemy movement */
	UFUNCTION(BlueprintCallable)
    void StartMovement();

	/** Move an Enemy to a Actor Target */
	UFUNCTION(BlueprintCallable)
    void MoveToTarget(AActor* Target);
	
	/** Move enemy to a Point */
	UFUNCTION(BlueprintCallable)
    void MoveToPoint(FVector& Point);

	/** Get point for enemy movement */
	UFUNCTION(BlueprintCallable)
    bool GetMovePoint(AActor* ToActor, FVector& OutResult);
		
	UFUNCTION()
    virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

//----------------------------------------------------------------------------------------------------------------------
// Combat
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** A range where an Enemy will attack a target */
	UPROPERTY(Category=Combat, EditDefaultsOnly, BlueprintReadWrite)
	class USphereComponent* CombatSphere;

	/** Is overlapping combat sphere? */
	UPROPERTY(Category=Combat, VisibleInstanceOnly, BlueprintReadWrite)
	bool bOverlappingCombatSphere;

	/** Is Attacking a Target */
	UPROPERTY(Category=Combat, VisibleInstanceOnly, BlueprintReadWrite)
	bool bAttacking;

	/** Target for Attacking */
	UPROPERTY(Category=Combat, VisibleInstanceOnly, BlueprintReadWrite)
	class AActor* CombatTarget;

	/** Combat Montage Class */
	UPROPERTY(Category=Combat, EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* CombatMontage;

	UPROPERTY(Category=Combat, EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDamageType> DamageTypeClass;

	/** Damage range applied to a Target */
	UPROPERTY(Category=Combat, EditDefaultsOnly, BlueprintReadWrite)
	FVector2D DamageRange;

	/** Attack a Target */
	UFUNCTION(BlueprintCallable)
    void AttackTarget(AActor* Target);

	/** Finish Attack a Target. Check for new Attack or follow */
	UFUNCTION(BlueprintCallable)
	void AttackFinish();

	/** Make damage to a Target */
	UFUNCTION(BlueprintCallable)
	void MakeDamage();

	/** find a hit */
	FHitResult AttackTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	UFUNCTION()
    virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
