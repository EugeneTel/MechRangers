// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MechRangers/Gameplay/Mech/MRMech.h"

#include "MRMechControlComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRMechControlComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	UMRMechControlComponent();

private:

	/** Controlled Mech */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	AMRMech* Mech;

public:
	/** Move Mech Forward */
	UFUNCTION(BlueprintCallable)
    void MoveForward(float Val);

	/** Turn Mech At rate */
	UFUNCTION(BlueprintCallable)
    void TurnAtRate(float Val);

	/** Attach a Pilot to a Mech */
	UFUNCTION(BlueprintCallable)
	void SitPilotIntoMech(AMRMech* NewMech);
};
