// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechRangers/MechRangersTypes.h"
#include "MechRangers/Gameplay/Interfaces/MRDamageTakerInterface.h"

#include "MRDestructibleObject.generated.h"

class UMRLivingActorComponent;

UCLASS()
class MECHRANGERS_API AMRDestructibleObject : public AActor, public IMRDamageTakerInterface
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual float InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Apply damage to component (for example destructible component) */
	virtual float ApplyDamageToComponent(float Damage, FDamageTakenData const& DamageTakenData, TWeakObjectPtr<class UPrimitiveComponent> DamagedComponent);
	
public:	
	// Sets default values for this actor's properties
	AMRDestructibleObject();
};
