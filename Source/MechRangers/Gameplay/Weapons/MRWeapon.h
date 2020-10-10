// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MRWeapon.generated.h"

struct FWeaponSpawnData;
class USoundCue;
class UMRSimpleAimComponent;
class APawn;

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

    /** inifite ammo for reloads */
    UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteAmmo;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteClip;

	/** max ammo */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 MaxAmmo;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 AmmoPerClip;

	/** initial clips */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 InitialClips;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float TimeBetweenShots;

	/** failsafe reload duration if weapon doesn't have any animation for it */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float NoAnimReloadDuration;

	/** defaults */
	FWeaponData()
	{
		bInfiniteAmmo = false;
		bInfiniteClip = false;
		MaxAmmo = 100;
		AmmoPerClip = 20;
		InitialClips = 4;
		TimeBetweenShots = 0.2f;
		NoAnimReloadDuration = 1.0f;
	}
};

/**
 * Weapon State
 */
enum class EWeaponState : uint8
{
	EWS_Idle,
	EWS_Firing,
	EWS_Reloading,
	EWS_Equipping
};

UCLASS(Abstract)
class MECHRANGERS_API AMRWeapon : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	AMRWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	/** Enable debug mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	bool bDebug;

	/** Weapon mesh  */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* MeshComp;

	/** Pawn owner */
	UPROPERTY(BlueprintReadWrite)
	APawn* MyPawn;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	FName MuzzleAttachPoint;

	/** Weapon Aim system */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UMRSimpleAimComponent* AimSystem;

public:

	UFUNCTION(BlueprintCallable)
	void SetOwningPawn(APawn* NewOwner);

	UFUNCTION(BlueprintCallable)
	void SetAimSystem(UMRSimpleAimComponent* InAimSystem);

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------
#pragma region Input

public:

	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();

	/** check if weapon can fire */
	bool CanFire() const;

#pragma endregion Input

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------
#pragma region WeaponUsage

protected:

	/** is weapon fire active? */
	uint32 bWantsToFire : 1;

	/** time of last successful weapon fire */
	float LastFireTime;
	
	/** Adjustment to handle frame rate affecting actual timer interval. */
	UPROPERTY(Transient)
	float TimerIntervalAdjustment;

	/** Whether to allow automatic weapons to catch up with shorter refire cycles */
	UPROPERTY(Config)
	bool bAllowAutomaticWeaponCatchup = true;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FWeaponData WeaponConfig;

	/** current weapon state */
	EWeaponState CurrentState;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

	
protected:
	
	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AMRWeapon::FireWeapon,);
	
	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** [local + server] handle weapon refire, compensating for slack time if the timer can't sample fast enough */
	void HandleReFiring();

	/** update weapon state */
	void SetWeaponState(EWeaponState NewState);
	
	/** determine current weapon state */
	void DetermineWeaponState();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();
	
#pragma endregion WeaponUsage

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
protected:
	/** is weapon currently equipped? */
	uint32 bEquipped : 1;

public:

	/** attaches weapon mesh to owner mesh */
	void AttachMesh(const FName SocketName) const;

	/** detaches weapon mesh from owner */
	void DetachMesh() const;
	
	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(APawn* NewOwner);

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory();

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

	void SetEquipped(const bool InEquipped);

//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** weapon is refiring */
	uint32 bRefiring;

	// @TODO: Replication
	/** current total ammo */
	UPROPERTY(Transient/*, Replicated*/)
	int32 CurrentAmmo;

	// @TODO: Replication
	/** current ammo - inside clip */
	UPROPERTY(Transient/*, Replicated*/)
	int32 CurrentAmmoInClip;

protected:

	/** check if weapon has infinite ammo (include owner's cheats) */
	bool HasInfiniteAmmo() const;

	/** check if weapon has infinite clip (include owner's cheats) */
	bool HasInfiniteClip() const;

public:
	
	enum class EAmmoType
	{
		EBullet,
        ERocket,
        EMax,
    };
	
	/** [server] add ammo */
	void GiveAmmo(int AddAmount);

	/** consume a bullet */
	void UseAmmo();

	/** query ammo type */
	virtual EAmmoType GetAmmoType() const
	{
		return EAmmoType::EBullet;
	}

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------

public:
	/** check if it's currently equipped */
	FORCEINLINE bool IsEquipped() const { return bEquipped; }
	
	/** weapon is being equipped by owner */
	virtual void OnEquip(const FWeaponSpawnData& WeaponSpawnData);

	/** weapon is now equipped by owner */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner */
	virtual void OnUnEquip();
	
//----------------------------------------------------------------------------------------------------------------------
// Effects
//----------------------------------------------------------------------------------------------------------------------
protected:

	/** firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
	UAudioComponent* FireAC;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UParticleSystem* MuzzleFX;

	/** Scale muzzle FX */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	FVector MuzzleFXScale;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSC;

	/** spawned component for second muzzle FX (Needed for split screen) */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSCSecondary;

	/** camera shake on firing */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	TSubclassOf<UCameraShake> FireCameraShake;

	/** force feedback effect to play when the weapon is fired */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	UForceFeedbackEffect *FireForceFeedback;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FireSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* FireFinishSound;
		
	/** Weapon Fire animation */
	UPROPERTY(Category=Animation, EditDefaultsOnly, BlueprintReadOnly)
	UAnimationAsset* FireAnim;

	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	uint32 bLoopedFireAnim : 1;

	/** is muzzle FX looped? */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	uint32 bLoopedMuzzleFX : 1;

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	uint32 bLoopedFireSound : 1;

	/** is fire animation playing? */
	uint32 bPlayingFireAnim : 1;

protected:
	/** Called in local and network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in local and network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();

	/** play weapon animations */
	void PlayWeaponAnimation(UAnimationAsset* Animation, const bool bIsLoopedAnim = false);

	/** stop playing weapon animations */
	void StopWeaponAnimation(const UAnimationAsset* Animation);
	
//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage Helpers
//----------------------------------------------------------------------------------------------------------------------

protected:
	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim();

	/** get the originating location for start damage */
	FVector GetDamageStartLocation(const FVector& AimDir) const;

	/** get damage end location depends on aiming */
	FVector GetDamageEndLocation() const;

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;
	
	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound) const;
	
};
