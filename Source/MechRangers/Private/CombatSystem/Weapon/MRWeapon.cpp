// Copyright PlatoSpace.com All Rights Reserved.

#include "CombatSystem/Weapon/MRWeapon.h"
#include "CombatSystem/Weapon/MRWeaponTypes.h"
#include "DrawDebugHelpers.h"
#include "Log.h"
#include "Kismet/GameplayStatics.h"
#include "MechRangers.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "CombatSystem/MRSimpleAimComponent.h"

// Sets default values
AMRWeapon::AMRWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create components
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	//MeshComp->SetupAttachment(GetRootComponent());
	SetRootComponent(MeshComp);

	bDebug = false;
	bLoopedMuzzleFX = false;
	MuzzleFXScale = FVector::OneVector;
	bEquipped = false;
	bWantsToFire = false;
	CurrentState = EWeaponState::EWS_Idle;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	LastFireTime = 0.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	// SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	// bReplicates = true;
	// bNetUseOwnerRelevancy = true;

}

// Called when the game starts or when spawned
void AMRWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMRWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMRWeapon::SetOwningPawn(APawn* NewOwner)
{
	if (MyPawn == nullptr || MyPawn != NewOwner)
	{
		MyPawn = NewOwner;
		SetOwner(NewOwner);
		SetInstigator(NewOwner);
	}
}

void AMRWeapon::SetAimSystem(UMRSimpleAimComponent* InAimSystem)
{
	AimSystem = InAimSystem;
}

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------
#pragma region Input

void AMRWeapon::StartFire()
{
	// @TODO: Implement server firing
	
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void AMRWeapon::StopFire()
{
	// TODO: Server side implementation
	
	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

bool AMRWeapon::CanFire() const
{
	// @TODO: Add checking is Pawn able fire

	// @TODO: Add checking for reloading

	const bool bStateOKToFire = ( ( CurrentState ==  EWeaponState::EWS_Idle ) || ( CurrentState == EWeaponState::EWS_Firing) );	
	return bStateOKToFire;
}



#pragma endregion Input

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------
#pragma region WeaponUsage

void AMRWeapon::HandleFiring()
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
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AMRWeapon::HandleReFiring, FMath::Max<float>(WeaponConfig.TimeBetweenShots + TimerIntervalAdjustment, SMALL_NUMBER), false);
		TimerIntervalAdjustment = 0.f;
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AMRWeapon::HandleReFiring()
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

void AMRWeapon::SetWeaponState(EWeaponState NewState)
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

void AMRWeapon::DetermineWeaponState()
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

void AMRWeapon::OnBurstStarted()
{
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
        LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AMRWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}	
}

void AMRWeapon::OnBurstFinished()
{
	// @TODO: Local checking
	
	StopSimulatingWeaponFire();
	
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;

	// reset firing interval adjustment
	TimerIntervalAdjustment = 0.0f;
}

void AMRWeapon::AttachMesh(USceneComponent* AttachToComponent, const FName SocketName) const
{

	// Remove and hide mesh
	DetachMesh();
	MeshComp->SetHiddenInGame(false);

	// @TODO: Check do we need id?
	MeshComp->AttachToComponent(AttachToComponent, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
}

void AMRWeapon::DetachMesh() const
{
	MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	MeshComp->SetHiddenInGame(true);
}

#pragma endregion WeaponUsage

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
void AMRWeapon::OnEnterInventory(APawn* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void AMRWeapon::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		SetOwningPawn(nullptr);
	}
}

bool AMRWeapon::IsAttachedToPawn() const
{
	// TODO: Pending Equip if needed
	return bEquipped; /*|| bPendingEquip;*/
}

void AMRWeapon::SetEquipped(const bool InEquipped)
{
	bEquipped = InEquipped;
}

//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------
#pragma region Ammo

bool AMRWeapon::HasInfiniteAmmo() const
{
	return WeaponConfig.bInfiniteAmmo;
}

bool AMRWeapon::HasInfiniteClip() const
{
	return WeaponConfig.bInfiniteClip;
}

void AMRWeapon::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	// @TODO: Bot checking

	// @TODO: Auto reloading
}

void AMRWeapon::UseAmmo()
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

void AMRWeapon::OnEquip(USceneComponent* AttachToComponent, const FWeaponSpawnData& WeaponSpawnData)
{
	AttachMesh(AttachToComponent, WeaponSpawnData.SocketName);
	
	bEquipped = true;
	
	// TODO: Implement
}

void AMRWeapon::OnEquipFinished()
{
	// TODO: Implement
}

void AMRWeapon::OnUnEquip()
{
	bEquipped = false;
	
	// TODO: Implement
}

#pragma endregion Ammo

//----------------------------------------------------------------------------------------------------------------------
// Effects
//----------------------------------------------------------------------------------------------------------------------

void AMRWeapon::SimulateWeaponFire()
{
	if (GetLocalRole() == ROLE_Authority && CurrentState != EWeaponState::EWS_Firing)
		return;

	if (MuzzleFX)
	{
		if (!bLoopedMuzzleFX || MuzzlePSC == nullptr)
		{
			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, MeshComp, MuzzleAttachPoint, FVector::ZeroVector, FRotator::ZeroRotator, MuzzleFXScale);
		}
	}

	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim, bLoopedFireAnim);
		bPlayingFireAnim = true;
	}

	if (bLoopedFireSound)
	{
		if (FireAC == nullptr)
		{
			FireAC = PlayWeaponSound(FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}

	// @TODO: Camera Shake
}

void AMRWeapon::StopSimulatingWeaponFire()
{
	if (bLoopedMuzzleFX )
	{
		if( MuzzlePSC != nullptr )
		{
			MuzzlePSC->DeactivateSystem();
			MuzzlePSC = nullptr;
		}
		if( MuzzlePSCSecondary != nullptr )
		{
			MuzzlePSCSecondary->DeactivateSystem();
			MuzzlePSCSecondary = nullptr;
		}
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = nullptr;

		PlayWeaponSound(FireFinishSound);
	}

	if (bLoopedFireAnim && bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}
}

void AMRWeapon::PlayWeaponAnimation(UAnimationAsset* Animation, const bool bIsLoopedAnim)
{
	if (MeshComp && Animation)
	{
		MeshComp->PlayAnimation(Animation, bIsLoopedAnim);
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMRWeapon::PlayWeaponAnimation - Animation is not set!"));
	}
}

void AMRWeapon::StopWeaponAnimation(const UAnimationAsset* Animation)
{
	MeshComp->Stop();
}


//----------------------------------------------------------------------------------------------------------------------
// Weapon Helpers
//----------------------------------------------------------------------------------------------------------------------



FVector AMRWeapon::GetAdjustedAim()
{
	if (AimSystem)
	{
		return AimSystem->GetForwardVector();
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("AMRWeapon::GetAdjustedAim - AimSystem is not set!"))
		return GetActorForwardVector();
	}

}

FVector AMRWeapon::GetDamageStartLocation(const FVector& AimDir) const
{
	// @TODO: Bots functionality

	// @TODO: Adjust trace if has blocker

	return GetMuzzleLocation();
}

FVector AMRWeapon::GetDamageEndLocation() const
{
	return AimSystem->GetTraceEndPoint();
}

FVector AMRWeapon::GetMuzzleLocation() const
{
	return MeshComp->GetSocketLocation(MuzzleAttachPoint);
}

FVector AMRWeapon::GetMuzzleDirection() const
{
	return MeshComp->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult AMRWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, this);
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bDebugQuery = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON_TRACE, TraceParams);

	// Draw debug line
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

UAudioComponent* AMRWeapon::PlayWeaponSound(USoundCue* Sound) const
{
	UAudioComponent* AC = nullptr;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}



