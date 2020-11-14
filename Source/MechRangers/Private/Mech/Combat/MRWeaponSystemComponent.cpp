// Copyright PlatoSpace.com All Rights Reserved.


#include "Mech/Combat/MRWeaponSystemComponent.h"
#include "Log.h"
#include "Mech/Combat/MRMechAimComponent.h"
#include "CombatSystem/Weapon/MRWeapon.h"
#include "Mech/MRMech.h"
#include "Mech/Structure/MRMechLoadoutDataAsset.h"


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
	OwnerMech = InMech;

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
	NewWeapon->SetOwningPawn(OwnerMech);
	NewWeapon->OnEquip(OwnerMech->GetMesh(), WeaponSpawnData);
	NewWeapon->SetAimSystem(MechArmedPart.AimSystem);
	MechArmedPart.Weapons.Add(WeaponSpawnData.Group, NewWeapon);
}

void UMRWeaponSystemComponent::SpawnAimSystem(FMechAim& MechAimConfig, FMechArmedPart& MechArmedPart)
{
	if (!MechAimConfig.MechAimClass || !IsValid(OwnerMech))
		return;

	UMRMechAimComponent* AimComponent = NewObject<UMRMechAimComponent>(OwnerMech, MechAimConfig.MechAimClass);

	if (IsValid(AimComponent))
	{
		AimComponent->SetupAttachment(OwnerMech->GetMesh(), MechAimConfig.Socket);
		AimComponent->RegisterComponent();

		MechArmedPart.AimSystem = AimComponent;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UMRWeaponSystemComponent::EquipWeapon - Can't spawn a Mech Anim System!"));
	}
}

bool UMRWeaponSystemComponent::GetArmedPart(const EMechPart MechPart, FMechArmedPart& OutArmedPart)
{
	if (ArmedParts.Num() > 0 && ArmedParts.Contains(MechPart))
	{
		OutArmedPart = ArmedParts[MechPart];
		return true;
	}

	return false;
}

void UMRWeaponSystemComponent::DestroyArmedPart(const EMechPart MechPart)
{
	if (ArmedParts.Contains(MechPart))
	{
		ArmedParts[MechPart].bIsActive = false;

		// Destroy weapons
		for(auto WeaponContainer : ArmedParts[MechPart].Weapons)
		{
			WeaponContainer.Get<1>()->StopFire();
			WeaponContainer.Get<1>()->Destroy();
		}

		// Destroy Aim System
		if (IsValid(ArmedParts[MechPart].AimSystem))
		{
			ArmedParts[MechPart].AimSystem->DestroyComponent();
		}

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

void UMRWeaponSystemComponent::StopAllWeaponsFire()
{
	for (auto ArmedPartContainer : ArmedParts)
	{
		for (auto FireWeaponContainer : ArmedPartContainer.Get<1>().Weapons)
		{
			FireWeaponContainer.Get<1>()->StopFire();
		}
	}
}

bool UMRWeaponSystemComponent::CanFire(const EMechPart MechPart, const EWeaponGroup WeaponGroup)
{
	// TODO: Check for alive etc.
	
	if (ArmedParts.Contains(MechPart) && ArmedParts[MechPart].bIsActive)
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

