// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MechRangers/Gameplay/Weapons/MRWeaponTypes.h"
#include "MechRangers/Gameplay/Mech/MRMechTypes.h"
#include "MRWeaponSystemComponent.generated.h"

class AMRMech;
class AMRMechAim;

USTRUCT(BlueprintType)
struct FMechArmedPart
{
	GENERATED_BODY()

	/** Armed Mech Part */ 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	EMechPart MechPart;

	/** List of weapons attached to the part */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TMap<EWeaponGroup, AMRWeapon*> Weapons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	AMRMechAim* AimSystem;

	/** Get weapon by Weapon Group from weapon list */
	AMRWeapon* GetWeapon(const EWeaponGroup Group)
	{
		if (Weapons.Contains(Group))
		{
			return Weapons[Group];
		}

		return nullptr;
	}
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRWeaponSystemComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UMRWeaponSystemComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	/** Owning mech */
	UPROPERTY(Category=MRWeaponSystem, BlueprintReadWrite, VisibleInstanceOnly)
	AMRMech* Mech;
	
	/** List of available armed parts for shooting */
	UPROPERTY(Category=MRWeaponSystem, BlueprintReadWrite, VisibleInstanceOnly)
	TMap<EMechPart, FMechArmedPart> ArmedParts;

public:
	
	/** Setup weapon system based on loadout */
	void Setup(AMRMech* InMech, TArray<FWeaponLoadout>& InWeaponLoadouts, TArray<FWeaponSlot>& InWeaponSlots, FMechAim& InMechAimConfig);

	/** Prepare Weapon Spawn Data from different sources */
	void MapWeaponSpawnData(const FWeaponLoadout& WeaponLoadout, const FWeaponSlot* WeaponSlot, OUT FWeaponSpawnData& OutWeaponSpawnData);
	
	/** Build Armed parts */
    void BuildArmedParts(TArray<FWeaponLoadout>& WeaponLoadouts, TArray<FWeaponSlot>& WeaponSlots, FMechAim& MechAimConfig);
	
	AMRWeapon* SpawnWeapon(const FWeaponSpawnData& WeaponSpawnData) const;

	void EquipWeapon(AMRWeapon* NewWeapon, const FWeaponSpawnData& WeaponSpawnData, FMechArmedPart& MechArmedPart);

	/** Spawn Mech Aim system */
	UFUNCTION(BlueprintCallable)
	void SpawnAimSystem(FMechAim& MechAimConfig, FMechArmedPart& MechArmedPart);


//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------

public:
	
	/** [local] starts weapon fire */
	void StartWeaponFire(const EMechPart MechPart, const EWeaponGroup WeaponGroup);

	/** [local] stops weapon fire */
	void StopWeaponFire(const EMechPart MechPart, const EWeaponGroup WeaponGroup);

	/** check if pawn can fire weapon */
	bool CanFire(const EMechPart MechPart, const EWeaponGroup WeaponGroup);
		
};
