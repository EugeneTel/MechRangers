// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "MechRangers/MechRangers.h"
#include "MechRangers/Gameplay/Mechs/BaseMech.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create components
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::SetOwnedLimb(ABaseLimb* NewLimb)
{
	if (OwnedLimb != NewLimb)
	{
		OwnedLimb = NewLimb;
		SetOwner(NewLimb);

		// Set Owned Mech as Instigator... need to think about it...
		SetInstigator(NewLimb->GetOwnedMech());
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------
#pragma region Input

void AWeaponBase::StartFire()
{
	// @TODO: Implement server firing
	
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void AWeaponBase::StopFire()
{
	// TODO: Server side implementation
	
	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

bool AWeaponBase::CanFire() const
{
	// @TODO: Add checking for Mech and Pilot

	// @TODO: Add checking for reloading

	const bool bStateOKToFire = ( ( CurrentState ==  EWeaponState::EWS_Idle ) || ( CurrentState == EWeaponState::EWS_Firing) );	
	return bStateOKToFire;
}



#pragma endregion Input

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------
#pragma region WeaponUsage

void AWeaponBase::HandleFiring()
{
	if ((CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	{
		// @TODO: Network and local checking 
		SimulateWeaponFire();

		FireWeapon();

		UseAmmo();
	}
	else
	{
		OnBurstFinished();
	}

	// @TODO: Reloading

	// @TODO: Out of ammo

	// @TODO: Server side

	// setup refire timer
	bRefiring = (CurrentState == EWeaponState::EWS_Firing && WeaponConfig.TimeBetweenShots > 0.0f);
	if (bRefiring)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeaponBase::HandleReFiring, FMath::Max<float>(WeaponConfig.TimeBetweenShots + TimerIntervalAdjustment, SMALL_NUMBER), false);
		TimerIntervalAdjustment = 0.f;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AWeaponBase::HandleReFiring()
{
	// Update TimerIntervalAdjustment
	UWorld* MyWorld = GetWorld();

	const float SlackTimeThisFrame = FMath::Max(0.0f, (MyWorld->TimeSeconds - LastFireTime) - WeaponConfig.TimeBetweenShots);

	if (bAllowAutomaticWeaponCatchup)
	{
		TimerIntervalAdjustment -= SlackTimeThisFrame;
	}

	HandleFiring();
}

void AWeaponBase::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	// Check for finish Firing 
	if (PrevState == EWeaponState::EWS_Firing && NewState != EWeaponState::EWS_Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	// Check for start Firing
	if (PrevState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
	{
		OnBurstStarted();
	}
}

void AWeaponBase::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::EWS_Idle;

	if (bEquipped)
	{
		// @TODO: Implement reload state

		if (bWantsToFire && CanFire())
		{
			NewState = EWeaponState::EWS_Firing;
		}
	}

	// @TODO: Implement equipment state

	SetWeaponState(NewState);
}

void AWeaponBase::OnBurstStarted()
{
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
        LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeaponBase::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}	
}

void AWeaponBase::OnBurstFinished()
{
	// @TODO: Local checking
	
	StopSimulatingWeaponFire();
	
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;

	// reset firing interval adjustment
	TimerIntervalAdjustment = 0.0f;
}

void AWeaponBase::AttachMeshToLimb(const ELimbSocket Socket) const
{
	if (!OwnedLimb)
		return;

	// Remove and hide mesh
	DetachMeshFromLimb();

	const FName AttachPoint = OwnedLimb->GetWeaponAttachPoint(Socket);
	MeshComp->SetHiddenInGame(false);
	MeshComp->AttachToComponent(OwnedLimb->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);

	// TODO: Remote controller implementation
}

void AWeaponBase::DetachMeshFromLimb() const
{
	MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	MeshComp->SetHiddenInGame(true);
}

#pragma endregion WeaponUsage

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
void AWeaponBase::OnEnterInventory(ABaseLimb* NewOwner)
{
	SetOwnedLimb(NewOwner);
}

void AWeaponBase::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		SetOwnedLimb(nullptr);
	}
}

bool AWeaponBase::IsAttachedToPawn() const
{
	// TODO: Pending Equip if needed
	return bEquipped; /*|| bPendingEquip;*/
}

//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------
#pragma region Ammo

bool AWeaponBase::HasInfiniteAmmo() const
{
	return WeaponConfig.bInfiniteAmmo;
}

bool AWeaponBase::HasInfiniteClip() const
{
	return WeaponConfig.bInfiniteClip;
}

void AWeaponBase::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	// @TODO: Bot checking

	// @TODO: Auto reloading
}

void AWeaponBase::UseAmmo()
{
	if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	// @TODO: Bot checking

	// @TODO: Player stats record
}

void AWeaponBase::OnEquip(const ELimbSocket Socket, AWeaponBase* LastWeapon)
{
	AttachMeshToLimb(Socket);
	
	bEquipped = true;
	
	// TODO: Implement
}

void AWeaponBase::OnEquipFinished()
{
	// TODO: Implement
}

void AWeaponBase::OnUnEquip()
{
	bEquipped = false;
	
	// TODO: Implement
}

#pragma endregion Ammo

//----------------------------------------------------------------------------------------------------------------------
// Effects
//----------------------------------------------------------------------------------------------------------------------

void AWeaponBase::SimulateWeaponFire()
{
}

void AWeaponBase::StopSimulatingWeaponFire()
{
}

//----------------------------------------------------------------------------------------------------------------------
// Weapon Helpers
//----------------------------------------------------------------------------------------------------------------------

FVector AWeaponBase::GetAdjustedAim() const
{
	return OwnedLimb->GetActorForwardVector();
}

FVector AWeaponBase::GetDamageStartLocation(const FVector& AimDir) const
{
	// @TODO: Bots functionality

	// @TODO: Adjust trace if has blocker

	return GetMuzzleLocation();
}

FVector AWeaponBase::GetMuzzleLocation() const
{
	return MeshComp->GetSocketLocation(MuzzleAttachPoint);
}

FVector AWeaponBase::GetMuzzleDirection() const
{
	return MeshComp->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult AWeaponBase::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo, const bool bDebug) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, this);
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bDebugQuery = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);

	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceFrom, TraceTo, FColor::Green, false, 5, 0.f, 1.f);

		if (Hit.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 12.f, 8, FColor::Red,false,5.f,0,1.f);
		}
	}

	return Hit;
}



