// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MRPilotInterface.h"
#include "GameFramework/Character.h"
#include "MechRangers/Gameplay/Mech/MRMech.h"

#include "MRPilotFP.generated.h"

class UMRMechControlComponent;

UCLASS()
class MECHRANGERS_API AMRPilotFP : public ACharacter, public IMRPilotInterface
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	AMRPilotFP();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//----------------------------------------------------------------------------------------------------------------------
// Components
//----------------------------------------------------------------------------------------------------------------------
private:

	UPROPERTY(Category=MRPilot, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UMRMechControlComponent* MechControl;

public:

	/** Get Mech Control Component */
	UFUNCTION(Category=MRPilot, BlueprintCallable)
	UMRMechControlComponent* GetMechControlComponent() const;

//----------------------------------------------------------------------------------------------------------------------
// Mech
//----------------------------------------------------------------------------------------------------------------------
private:

	/** A Mech owned by the Pilot */
	UPROPERTY(Category=MRPilot, VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	AMRMech* Mech;
	
protected:

	/** Sit Pilot Into Mech. Inteface method implementation */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SitIntoMech(AMRMech* NewMech);

	/** Input Mech Move Forward */
	UFUNCTION(Category=MRPilot, BlueprintCallable)
	void MechMoveForward(float Val);

	/** Input Mech Turn Rate */
	UFUNCTION(Category=MRPilot, BlueprintCallable)
    void MechTurnAtRate(float Val);

	
	
};
