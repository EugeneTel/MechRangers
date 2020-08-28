// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MRMechLivingComponent.h"
#include "GameFramework/Character.h"
#include "MRMech.generated.h"

UCLASS()
class MECHRANGERS_API AMRMech : public ACharacter
{
	GENERATED_BODY()

protected:

	/** Models the various health containing parts of a Mech, including interactions between health components. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRMechLivingComponent* LivingComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Sets default values for this character's properties
	AMRMech(const class FObjectInitializer& ObjectInitializer);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
