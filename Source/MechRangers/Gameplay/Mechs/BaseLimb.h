// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "BaseLimb.generated.h"

/** Limb Type */
UENUM(BlueprintType)
enum class ELimbType : uint8
{
	ELT_Left UMETA(DisplayName = "Left"),
    ELT_Right UMETA(DisplayName = "Right")
};

/** Limb Crosshair Type */
UENUM(BlueprintType)
enum class ELimbCrosshairType : uint8
{
	ELCT_None UMETA(DisplayName = "None"),
	ELCT_Basic UMETA(DisplayName = "Basic")
};

UCLASS()
class MECHRANGERS_API ABaseLimb : public AActor
{
	GENERATED_BODY()

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	ABaseLimb();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	/** Limb's Mesh component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	/** Saved Trace End Point from the last Trace */
	UPROPERTY(BlueprintReadWrite)
	FVector TraceEndPoint;

	/** Crosshair Type of the Limb */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ELimbCrosshairType CrosshairType;

	/** Controlling speed (rotate, move etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ControlSpeed;

public:

	/** Get Saved Trace End Point from the last Trace */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector& GetTraceEndPoint() { return TraceEndPoint; }

	/** Get Crosshair Type of the current Limb */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ELimbCrosshairType GetCrosshairType() const { return CrosshairType; }
	
	/** Control the Limb (rotate, move etc.) */
	UFUNCTION(BlueprintCallable)
	void Control(float AxisX, float AxisY);

	/** Make a line trace */
	UFUNCTION(BlueprintCallable)
	void Trace();

};
