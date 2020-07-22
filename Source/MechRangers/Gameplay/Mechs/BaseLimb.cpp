// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseLimb.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "MechRangers/UI/HUD/CrosshairBase.h"
#include "MechRangers/Gameplay/Characters/Pilot/PilotCharacter.h"
#include "MechRangers/Gameplay/Mechs/BaseMech.h"
#include "MechRangers/Gameplay/Weapons/WeaponBase.h"
#include "Log.h"

// Sets default values
ABaseLimb::ABaseLimb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create Components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	// Setup Defaults
	ControlSpeed = 20.f;
	CrosshairType = ELimbCrosshairType::ELCT_None;
}

void ABaseLimb::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// TODO: Check Authority

	// Needs to happen after character is added to repgraph
	GetWorldTimerManager().SetTimerForNextTick(this, &ABaseLimb::SpawnDefaultInventory);
}

// Called when the game starts or when spawned
void ABaseLimb::BeginPlay()
{
	Super::BeginPlay();

	SpawnCrosshair();
}

// Called every frame
void ABaseLimb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Trace();
}

void ABaseLimb::SetOwnedMech(ABaseMech* NewMech)
{
	OwnedMech = NewMech;
}

ABaseMech* ABaseLimb::GetOwnedMech() const
{
	return OwnedMech;
}

UStaticMeshComponent* ABaseLimb::GetMesh() const
{
	return MeshComp;
}

void ABaseLimb::Control(const float AxisX, const float AxisY)
{
	const float RotationSpeed = ControlSpeed * GetWorld()->GetDeltaSeconds(); 
	AddActorLocalRotation(FRotator(AxisY * RotationSpeed, AxisX * RotationSpeed, 0.f));
}

void ABaseLimb::Trace()
{
	const float TraceLength = 10000.f;
	const FVector StartPoint = MeshComp->GetSocketLocation(FName("Muzzle"));
	FVector EndPoint = StartPoint + MeshComp->GetForwardVector() * TraceLength;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bDebugQuery = true;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		EndPoint = HitResult.ImpactPoint;
	}

	DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, -1, 0.f, 2.f);

	// Save trace end point for future usage
	TraceEndPoint = EndPoint;

	// Update Crosshair Position
	if (Crosshair)
	{
		Crosshair->SetActorLocation(EndPoint);

		ABaseMech* Mech = Cast<ABaseMech>(GetOwner());
		if (Mech)
		{
			Crosshair->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Crosshair->GetActorLocation(), Mech->GetOwnedPilot()->GetActorLocation()));
		}
	}
}

void ABaseLimb::SpawnCrosshair()
{
	if (!CrosshairClass)
		return;

	FVector SpawnLocation = GetActorLocation();
	FActorSpawnParameters SpawnParams;
	Crosshair = GetWorld()->SpawnActor<ACrosshairBase>(CrosshairClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
}

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------

void ABaseLimb::SetCurrentWeapon(AWeaponBase* NewWeapon, AWeaponBase* LastWeapon)
{
	AWeaponBase* LocalLastWeapon = nullptr;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwnedLimb(this);	// Make sure weapon's Limb is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewWeapon->OnEquip(LastWeapon);
	}
}

void ABaseLimb::SpawnDefaultInventory()
{
	if (SocketsConfig.Num() == 0)
		return;
	
	for (const TPair<ELimbSocket, FLimbSocketData>& SocketConfig : SocketsConfig)
	{
		FLimbSocketData SocketData = SocketConfig.Value;

		if (SocketData.DefaultWeapon)
		{
			const ELimbSocket Socket = SocketConfig.Key;

			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(SocketData.DefaultWeapon, SpawnInfo);
			AddWeapon(Socket, NewWeapon);
		}
	}
}

void ABaseLimb::DestroyInventory()
{
}

void ABaseLimb::AddWeapon(ELimbSocket Socket, AWeaponBase* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Weapon->AttachMeshToLimb(Socket);
		Inventory.Add(Socket, Weapon);
		Weapon->StartFire();
	}
}

void ABaseLimb::EquipWeapon(const ELimbSocket Socket)
{
	if (!Inventory.Contains(Socket))
		return;

	// TODO: Server implementation
	SetCurrentWeapon(Inventory[Socket]);
}

FName ABaseLimb::GetWeaponAttachPoint(const ELimbSocket Socket) const
{
	FName AttachPoint;
	
	if (SocketsConfig.Contains(Socket))
	{
		AttachPoint = SocketsConfig[Socket].AttachPoint;
	}

	return AttachPoint;
}
