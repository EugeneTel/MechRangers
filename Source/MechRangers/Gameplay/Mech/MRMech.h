// Copyright PlatoSpace.com. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MechComponents/MRMechLivingComponent.h"
#include "GameFramework/Character.h"
#include "MechDataAssets/MRMechLoadoutDataAsset.h"

#include "MRMech.generated.h"

UCLASS()
class MECHRANGERS_API AMRMech : public ACharacter
{
	GENERATED_BODY()

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Sets default values for this character's properties
	AMRMech(const class FObjectInitializer& ObjectInitializer);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//----------------------------------------------------------------------------------------------------------------------
// Components
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** Models the various health containing parts of a Mech, including interactions between health components. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMRMechLivingComponent* LivingComponent;

//----------------------------------------------------------------------------------------------------------------------
// Configs
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** Loadout asset for the Mech. Every Mech must have it! */ 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Mech)
	UMRMechLoadoutDataAsset* MechLoadoutAsset;
};
