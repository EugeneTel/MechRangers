// Copyright PlatoSpace.com All Rights Reserved.


#include "MRWeaponSystemComponent.h"
#include "MechRangers/Gameplay/Weapons/MRWeapon.h"
#include "../MRMech.h"
#include "../MRMechAim.h"
#include "../MechDataAssets/MRMechLoadoutDataAsset.h"
#include "Log.h"

// Sets default values for this component's properties
UMRWeaponSystemComponent::UMRWeaponSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMRWeaponSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMRWeaponSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMRWeaponSystemComponent::Setup(AMRMech* InMech, TArray<FWeaponLoadout>& InWeaponLoadouts, TArray<FWeaponSlot>& InWeaponSlots, FMechAim& InMechAimConfig)
{
	Mech = InMech;

	// Build mech armed parts
	BuildArmedParts(InWeaponLoadouts, InWeaponSlots, InMechAimConfig);

	
}

void UMRWeaponSystemComponent::MapWeaponSpawnData(const FWeaponLoadout& WeaponLoadout, const FWeaponSlot* WeaponSlot, OUT FWeaponSpawnData& OutWeaponSpawnData)
{
	OutWeaponSpawnData.Name = WeaponLoadout.WeaponAsset->Name;
	OutWeaponSpawnData.Group = WeaponLoadout.WeaponGroup;
	OutWeaponSpawnData.Size = WeaponLoadout.WeaponAsset->WeaponSize;
	OutWeaponSpawnData.Type = WeaponLoadout.WeaponAsset->WeaponType;
	OutWeaponSpawnData.MechPart =  WeaponSlot->MechPart;
	OutWeaponSpawnData.SocketName = WeaponSlot->SocketName;
	OutWeaponSpawnData.WeaponClass = WeaponLoadout.WeaponAsset->WeaponClass;
	OutWeaponSpawnData.HardpointSlotID = WeaponLoadout.HardpointSlotID;

	if (OutWeaponSpawnData.WeaponClass == nullptr || OutWeaponSpawnData.SocketName.IsNone() || OutWeaponSpawnData.HardpointSlotID.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("UMRWeaponSystemComponent::MapWeaponSpawnData - Weapon Spawn Data is not correct! Some fields aren't filled"));
	}
}

void UMRWeaponSystemComponent::BuildArmedParts(TArray<FWeaponLoadout>& WeaponLoadouts, TArray<FWeaponSlot>& WeaponSlots, FMechAim& MechAimConfig)
{
	for (FWeaponLoadout& WeaponLoadout : WeaponLoadouts)
	{
		// Try to find weapon slot for weapon loadout
		FWeaponSlot* WeaponSlot = WeaponSlots.FindByPredicate([WeaponLoadout](FWeaponSlot& Slot)
		{
			return Slot.HardpointSlotID.IsEqual(WeaponLoadout.HardpointSlotID);
		}) ;

		if (WeaponSlot->SocketName == NAME_None)
		{
			UE_LOG(LogTemp, Error, TEXT("UMRWeaponSystemComponent::BuildArmedParts - Weapon Slot empty or not found!"));
			continue;
		}

		// Create a new Armed Part if needed and Aim system
		if (!ArmedParts.Contains(WeaponSlot->MechPart))
		{
			FMechArmedPart ArmedPart;
			ArmedPart.MechPart = WeaponSlot->MechPart;	

			MechAimConfig.Socket = WeaponSlot->SocketName;
		
			// Add Mech Aim system
			SpawnAimSystem(MechAimConfig, ArmedPart);

			ArmedParts.Add(WeaponSlot->MechPart, ArmedPart);
		}

		// Create weapons
		FWeaponSpawnData WeaponSpawnData;
		MapWeaponSpawnData(WeaponLoadout, WeaponSlot, WeaponSpawnData);
		AMRWeapon* NewWeapon = SpawnWeapon(WeaponSpawnData);
		if (NewWeapon)
		{
			EquipWeapon(NewWeapon, WeaponSpawnData, ArmedParts[WeaponSlot->MechPart]);
		} else
		{
			UE_LOG(LogTemp, Error, TEXT("UMRWeaponSystemComponent::BuildArmedParts - Can't spawn NewWeapon"))
		}
	}
}

AMRWeapon* UMRWeaponSystemComponent::SpawnWeapon(const FWeaponSpawnData& WeaponSpawnData) const
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AMRWeapon* NewWeapon = GetWorld()->SpawnActor<AMRWeapon>(WeaponSpawnData.WeaponClass, SpawnInfo);
	return  NewWeapon;
}

void UMRWeaponSystemComponent::EquipWeapon(AMRWeapon* NewWeapon, const FWeaponSpawnData& WeaponSpawnData, FMechArmedPart& MechArmedPart)
{
	NewWeapon->SetOwningPawn(Mech);
	NewWeapon->OnEquip(WeaponSpawnData);
	//NewWeapon->SetAimSystem(MechArmedPart.AimSystem);
	MechArmedPart.Weapons.Add(WeaponSpawnData.Group, NewWeapon);
}

void UMRWeaponSystemComponent::SpawnAimSystem(FMechAim& MechAimConfig, FMechArmedPart& MechArmedPart)
{
	if (!MechAimConfig.MechAimClass)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = Mech;
	SpawnParams.Instigator = Mech;
	
	MechArmedPart.AimSystem = GetWorld()->SpawnActor<AMRMechAim>(MechAimConfig.MechAimClass, SpawnParams);

	if (MechArmedPart.AimSystem)
	{
		MechArmedPart.AimSystem->AttachToComponent(Mech->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MechAimConfig.Socket);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UMRWeaponSystemComponent::EquipWeapon - Can't spawn a Mech Anim System!"));
	}
}

void UMRWeaponSystemComponent::StartWeaponFire(const EMechPart MechPart, const EWeaponGroup WeaponGroup)
{
	if (ArmedParts.Contains(MechPart))
	{
		AMRWeapon* FireWeapon = ArmedParts[MechPart].GetWeapon(WeaponGroup);

		if (FireWeapon)
		{
			ULog::Success("StartWeaponFire");
			FireWeapon->StartFire();
		}
	}
}

void UMRWeaponSystemComponent::StopWeaponFire(const EMechPart MechPart, const EWeaponGroup WeaponGroup)
{
	if (ArmedParts.Contains(MechPart))
	{
		AMRWeapon* FireWeapon = ArmedParts[MechPart].GetWeapon(WeaponGroup);

		if (FireWeapon)
		{
			FireWeapon->StopFire();
		}
	}
}

bool UMRWeaponSystemComponent::CanFire(const EMechPart MechPart, const EWeaponGroup WeaponGroup)
{
	// TODO: Check for alive etc.
	
	if (ArmedParts.Contains(MechPart))
	{
		AMRWeapon* FireWeapon = ArmedParts[MechPart].GetWeapon(WeaponGroup);

		if (FireWeapon)
		{
			// TODO: Check ammo, heating etc.

			return true;
		}
	}

	return false;
}

