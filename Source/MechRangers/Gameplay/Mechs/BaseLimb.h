// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseLimb.generated.h"

class ACrosshairBase;
class ABaseMech;
class AWeaponBase;

/** Limb Type */
UENUM(BlueprintType)
enum class ELimbType : uint8
{
	ELT_Left UMETA(DisplayName = "Left"),
    ELT_Right UMETA(DisplayName = "Right")
};

/** Limb Crosshair Type */
UENUM(BlueprintType)
enum class ELimbCrosshairType : uint8
{
	ELCT_None UMETA(DisplayName = "None"),
	ELCT_Basic UMETA(DisplayName = "Basic")
};

/** Limb sockets for weapon attachment */
UENUM(BlueprintType)
enum class ELimbSocket : uint8
{
	ELS_Primary UMETA(DisplayName = "Primary"),
	ELS_Secondary UMETA(DisplayName = "Secondary")
};

/** Limb Socket data used for weapon spawning and attaching */ 
USTRUCT(BlueprintType)
struct FLimbSocketData
{
	GENERATED_USTRUCT_BODY()

	/** Mesh Socket name for attachment */
	UPROPERTY(EditDefaultsOnly)
	FName AttachPoint;

	/** Default Weapon will be spawned and attached */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeaponBase> DefaultWeapon;

	/** defaults */
	FLimbSocketData()
	{
		AttachPoint = NAME_None;
		DefaultWeapon = nullptr;
	}
	
};

UCLASS()
class MECHRANGERS_API ABaseLimb : public AActor
{
	GENERATED_BODY()

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Sets default values for this actor's properties
	ABaseLimb();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	/** Limb's Mesh component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComp;

	/** Mech owned by the Limb */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	ABaseMech* OwnedMech;

	/** Limb sockets configuration. Used for spawning weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<ELimbSocket, FLimbSocketData> SocketsConfig;

	/** weapons in inventory */
	// @TODO: Replication
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	TMap<ELimbSocket, AWeaponBase*> Inventory;

	/** currently equipped weapons at limb */
	// @TODO: Replication
	UPROPERTY(Transient/*, ReplicatedUsing = OnRep_CurrentWeapon*/)
	TMap<ELimbSocket, AWeaponBase*> CurrentWeapons;

	/** Saved Trace End Point from the last Trace */
	UPROPERTY(BlueprintReadWrite)
	FVector TraceEndPoint;

	/** Crosshair instance for the current limb */
	UPROPERTY(BlueprintReadWrite)
	ACrosshairBase* Crosshair;

	/** Crosshair Type of the Limb */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ELimbCrosshairType CrosshairType;

	/** Crosshair actor class with widget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ACrosshairBase> CrosshairClass;

	/** Controlling speed (rotate, move etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ControlSpeed;

public:

	/** Get Saved Trace End Point from the last Trace */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector& GetTraceEndPoint() { return TraceEndPoint; }

	/** Get Crosshair Type of the current Limb */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ELimbCrosshairType GetCrosshairType() const { return CrosshairType; }

	/** Set the Mech Owned by the Limb */
	UFUNCTION(BlueprintCallable)
	void SetOwnedMech(ABaseMech* NewMech);

	/** Get the Mech Owned by the Limb */
	UFUNCTION(BlueprintCallable)
    ABaseMech* GetOwnedMech() const;

	/** Get Mesh component */
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetMesh() const;
	
	/** Control the Limb (rotate, move etc.) */
	UFUNCTION(BlueprintCallable)
	void Control(float AxisX, float AxisY);

	/** Make a line trace */
	UFUNCTION(BlueprintCallable)
	void Trace();

	/** Spawn Crosshair from the class */
	UFUNCTION(BlueprintCallable)
	void SpawnCrosshair();

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
public:
	/** get weapon attach point */
	FName GetWeaponAttachPoint(ELimbSocket Socket) const;
	
protected:
	
	/** updates current weapon for selected socket in limb */
	UFUNCTION(BlueprintCallable)
    void SetCurrentWeapon(ELimbSocket Socket, AWeaponBase* NewWeapon, AWeaponBase* LastWeapon = nullptr);
	
	/** [server] spawns default inventory */
	void SpawnDefaultInventory();

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();

	/** [server] add weapon to inventory */
	void InventoryAdd(ELimbSocket Socket, AWeaponBase* Weapon);

	/** [server + local] equips weapon from inventory */
	void EquipWeapon(ELimbSocket Socket);

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------

public:
	/** [local] starts weapon fire */
	void StartWeaponFire(const ELimbSocket Socket);

	/** [local] stops weapon fire */
	void StopWeaponFire(const ELimbSocket Socket);

	/** check if pawn can fire weapon */
	bool CanFire(const ELimbSocket Socket) const;
	
};
