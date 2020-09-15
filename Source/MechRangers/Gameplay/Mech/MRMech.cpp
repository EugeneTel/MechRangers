// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMech.h"
#include "Components/CapsuleComponent.h"
#include "MechComponents/MRMechMovementComponent.h"
#include "MechComponents/MRMechLivingComponent.h"
#include "MechDataAssets/MRMechLoadoutDataAsset.h"
#include "MechDataAssets/MRMechModelDataAsset.h"
#include "MechDataAssets/MRMechHardpointDataAsset.h"
#include "MRMechAnimInstance.h"
#include "MRMechCockpit.h"
#include "Log.h"

// Sets default values
AMRMech::AMRMech(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UMRMechMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Defaults
	LivingComponent = CreateDefaultSubobject<UMRMechLivingComponent>(TEXT("LivingComponent"));
	WeaponSystem = CreateDefaultSubobject<UMRWeaponSystemComponent>(TEXT("WeaponSystem"));

	// Setup Mech
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::Disabled;

	// Setup Defaults
	bIsCombatMode = false;
}

// Called when the game starts or when spawned
void AMRMech::BeginPlay()
{
	Super::BeginPlay();

	check(MechLoadoutAsset);

	// Try to setup default Loadout if not set on creation
	if (!MechLoadout.MechModelAsset && MechLoadoutAsset)
	{
		MechLoadout = MechLoadoutAsset->GetLoadout();
		MechModelData = MechLoadout.MechModelAsset->GetModelData();

		ConstructMech();
	}
}

// Called every frame
void AMRMech::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UMRWeaponSystemComponent* AMRMech::GetWeaponSystem() const
{
	return WeaponSystem;
}

void AMRMech::SetLoadout(FMechLoadout NewLoadout)
{
	MechLoadout = NewLoadout;
	MechModelData = MechLoadout.MechModelAsset->GetModelData();
}

void AMRMech::ConstructMech()
{
	// Setup Skeletal Mesh
	USkeletalMeshComponent* MechMesh = GetMesh();
	MechMesh->SetSkeletalMesh(MechModelData.SkeletalMesh);
	MechMesh->SetAnimClass(MechModelData.AnimClass);

	// Setup Capsule
	UMRMechCapsuleDataAsset* CapsuleDataAsset = MechModelData.CapsuleAsset;
	GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleDataAsset->HalfHeight);
	GetCapsuleComponent()->SetCapsuleRadius(CapsuleDataAsset->Radius);

	// Add mesh offset if needed
	if (CapsuleDataAsset->MeshOffset)
	{
		GetMesh()->AddLocalOffset(FVector(0.f, 0.f, CapsuleDataAsset->MeshOffset));
	}

	// Setup Movement params
	GetCharacterMovement()->MaxStepHeight = CapsuleDataAsset->MaxStepHeight;
	GetCharacterMovement()->SetWalkableFloorAngle(CapsuleDataAsset->WalkableFloorAngle);

	// Setup Cockpit
	Cockpit = SpawnCockpit(MechModelData.VRCockpit);

	const auto WeaponHardpointAsset = MechModelData.WeaponHardpointAsset;
	if (!WeaponHardpointAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponHardpointAsset is not set in MechModelData"));
		return;
	}

	if (MechLoadout.WeaponLoadouts.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponLoadouts is not set in MechLoadout"));
		return;
	}

	// Setup Weapons System
	WeaponSystem->Setup(this, MechLoadout.WeaponLoadouts, MechModelData.WeaponHardpointAsset->MechHardpoints.WeaponSlots, MechModelData.MechAimConfig);
}

AMRMechCockpit* AMRMech::SpawnCockpit(const FMechCockpit CockpitData)
{
	if (!CockpitData.CockpitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't spawn a Cockpit! CockpitClass is not specified in CockpitData"));
		return nullptr;
	}

	if (CockpitData.Socket.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("Can't spawn a Cockpit! Cockpit Socket is not specified in CockpitData"));
		return nullptr;
	}

	const FActorSpawnParameters SpawnParams;
	AMRMechCockpit* NewMechCockpit = GetWorld()->SpawnActor<AMRMechCockpit>(CockpitData.CockpitClass, SpawnParams);

	if (NewMechCockpit)
	{
		//NewMechCockpit->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, CockpitData.Socket);
		NewMechCockpit->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CockpitData.Socket);
		return NewMechCockpit;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't spawn a Cockpit!"));
		return nullptr;
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Controlling
//----------------------------------------------------------------------------------------------------------------------

void AMRMech::SetCombatMode(bool const Val)
{
	bIsCombatMode = Val;
}

void AMRMech::MoveForward(float Val)
{
	if (Controller && Val != 0.f)
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void AMRMech::TurnAtRate(float Val)
{
	if (Val == 0)
		return;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	const float BaseTurnRate = 40.f;
	const float Delta = Val * BaseTurnRate * GetWorld()->GetDeltaSeconds();
	AddActorWorldRotation(FRotator(0.f, Delta, 0.f));

	// Adjust camera to Mech Rotation
	Controller->SetControlRotation(GetActorRotation());
}

void AMRMech::AddArmRotator(FRotator& ArmRotator, const FRotator& AddRot)
{
	const float BaseRotRate = 2.f;
	ArmRotator += AddRot * (BaseRotRate * GetWorld()->GetDeltaSeconds());

	const float RotateLimit = 1.f;
	if (FMath::Abs(ArmRotator.Pitch) > RotateLimit)
	{
		ArmRotator.Pitch = ArmRotator.Pitch > 0 ? RotateLimit : RotateLimit * -1;
	}

	if (FMath::Abs(ArmRotator.Yaw) > RotateLimit)
	{
		ArmRotator.Yaw = ArmRotator.Yaw > 0 ? RotateLimit : RotateLimit * -1;
	}
}

void AMRMech::AddArmLeftRotator(const FRotator& Rot)
{
	AddArmRotator(ArmLeftRotator, Rot);
}

void AMRMech::AddArmRightRotator(const FRotator& Rot)
{
	AddArmRotator(ArmRightRotator, Rot);
}
