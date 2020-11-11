// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMech.h"
#include "Log.h"
#include "Components/CapsuleComponent.h"
#include "MechComponents/MRMechMovementComponent.h"
#include "MechComponents/MRMechLivingComponent.h"
#include "MechDataAssets/MRMechLoadoutDataAsset.h"
#include "MechDataAssets/MRMechModelDataAsset.h"
#include "MechDataAssets/MRMechHardpointDataAsset.h"
#include "MechDataAssets/MRMechCapsuleDataAsset.h"
#include "MRMechAnimInstance.h"
#include "Cockpit/MRMechCockpit.h"
#include "MechRangers/Modes/MRGameMode.h"
#include "Engine/World.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMRMech::AMRMech(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UMRMechMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Defaults
	LivingComponent = CreateDefaultSubobject<UMRMechLivingComponent>(TEXT("LivingComponent"));
	WeaponSystemComponent = CreateDefaultSubobject<UMRWeaponSystemComponent>(TEXT("WeaponSystemComponent"));
	HitReactionComponent = CreateDefaultSubobject<UMRMechHitReactionComponent>(TEXT("HitReactionComponent"));

	// Setup Mech
	bUseControllerRotationYaw = false;
	AutoPossessAI = EAutoPossessAI::Disabled;
	GetCapsuleComponent()->SetCollisionProfileName(FName("MechPawn"));

	// Setup Defaults
	bIsCombatMode = false;
	bManipulatorLeftHeld = false;
	bManipulatorRightHeld = false;
	GameplayTeam = EGameplayTeam::Player;
	AgroChance = 0.5;
	ControlType = EMechControlType::None;
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

float AMRMech::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		//ULog::Success("Mech Takes Damage", LO_Both);

		//LivingComponent->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
		//HitReactionComponent->PlayHit(Damage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : nullptr, DamageCauser);
	}

	return ActualDamage;
}

float AMRMech::InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		if (LivingComponent)
		{
			LivingComponent->TakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);	
		}
	}

	return ActualDamage;
}

float AMRMech::InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//const float ActualDamage = Super::InternalTakeRadialDamage(Damage, RadialDamageEvent, EventInstigator, DamageCauser);
	if (Damage > 0.f)
	{
		if (LivingComponent)
		{
			return LivingComponent->TakeRadialDamage(Damage, RadialDamageEvent, EventInstigator, DamageCauser);	
		}
	}

	return 0.f;
}

UMRWeaponSystemComponent* AMRMech::GetWeaponSystem() const
{
	return WeaponSystemComponent;
}

EGameplayTeam AMRMech::GetGameplayTeam() const
{
	return GameplayTeam;
}

float AMRMech::GetAgroChance() const
{
	return AgroChance;
}

bool AMRMech::Alive() const
{
	return LivingComponent->GetHealthState() == EHealthState::EHS_Healthy || LivingComponent->GetHealthState() == EHealthState::EHS_Damaged;
}

void AMRMech::SetupControlType()
{
	// Set default control type if not set
	if (ControlType == EMechControlType::None)
	{
		ControlType = Cast<AMRGameMode>(GetWorld()->GetAuthGameMode())->IsVR() ? EMechControlType::VR : EMechControlType::FP;
	}
}

void AMRMech::SetLoadout(const FMechLoadout& NewLoadout)
{
	MechLoadout = NewLoadout;
	MechModelData = MechLoadout.MechModelAsset->GetModelData();
}

void AMRMech::ConstructMech()
{
	SetupControlType();
	
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
	Cockpit = SpawnCockpit(MechModelData.Cockpit);

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
	if (IsValid(WeaponSystemComponent))
	{
		WeaponSystemComponent->Setup(this, MechLoadout.WeaponLoadouts, MechModelData.WeaponHardpointAsset->MechHardpoints.WeaponSlots, MechModelData.MechAimConfig);
	}

}

AMRMechCockpit* AMRMech::SpawnCockpit(FMechCockpit& CockpitData)
{
	if (!CockpitData.CockpitClasses.Contains(ControlType) || !IsValid(CockpitData.CockpitClasses[ControlType]))
	{
		UE_LOG(LogTemp, Error, TEXT("Can't spawn a Cockpit! Cockpit Socket is not specified in CockpitData"));
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	AMRMechCockpit* NewMechCockpit = GetWorld()->SpawnActor<AMRMechCockpit>(CockpitData.CockpitClasses[ControlType], SpawnParams);

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

bool AMRMech::HasPilot() const
{
	return ControlType == EMechControlType::FP || ControlType == EMechControlType::VR;
}

IMRDamageTakerInterface::FOnDeath& AMRMech::OnDeath()
{
	return OnDeathEvent;
}

//----------------------------------------------------------------------------------------------------------------------
// Controlling
//----------------------------------------------------------------------------------------------------------------------

void AMRMech::SetCombatMode(bool const Val)
{
	bIsCombatMode = Val;
}

void AMRMech::MoveForward(const float Val)
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

void AMRMech::TurnAtRate(const float Val)
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
	const float BaseRotRate = 0.5f;
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

void AMRMech::SetManipulatorHeld(const EMechPart MechPart, const bool IsHeld)
{
	if (MechPart == EMechPart::EMP_LeftArm)
	{
		bManipulatorLeftHeld = IsHeld;
	} else if (MechPart == EMechPart::EMP_RightArm)
	{
		bManipulatorRightHeld = IsHeld;
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("AMRMech::SetManipulatorHeld - MechPart is not supported."));
	}

	if (bManipulatorRightHeld == true || bManipulatorLeftHeld == true)
	{
		bIsCombatMode = true;
	} else
	{
		bIsCombatMode = false;
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

void AMRMech::SetMechPartRotator(const EMechPart MechPart, const FRotator& Rot)
{
	if (MechPart == EMechPart::EMP_LeftArm)
	{
		ArmLeftRotator = Rot;
	} else if (MechPart == EMechPart::EMP_RightArm)
	{
		ArmRightRotator = Rot;
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetMechPartRotator - MechPart is not supported!"));
	}
}

void AMRMech::DestroyPart(const EMechPart MechPart, const EHealthState HealthState)
{
	FMechPartUpdateData MechPartUpdateData;
	if (GetMechPartUpdateData(MechPart, HealthState, MechPartUpdateData))
	{
		// VFX
		for (FMechParticleSpawnData& Effect : MechPartUpdateData.Effects)
		{
			SpawnParticle(Effect);
		}

		// SFX
		for (FMechSoundSpawnData& Sound : MechPartUpdateData.AudioData)
		{
			SpawnSound(Sound);
		}
	}
	
	FMechBonesToHide BonesToHide;
	if (GetBonesToHide(MechPart, HealthState, BonesToHide))
	{
		for(FName& BoneName : BonesToHide.BoneNames)
		{
			GetMesh()->HideBoneByName(BoneName, EPhysBodyOp::PBO_None);
		}
	}

	WeaponSystemComponent->DestroyArmedPart(MechPart);
}

void AMRMech::ReplacePart(const EMechPart MechPart, const EHealthState HealthState)
{
	FMechPartUpdateData MechPartUpdateData;
	if (GetMechPartUpdateData(MechPart, HealthState, MechPartUpdateData))
	{
		for(FMechReplacementMesh& PartReplacement : MechPartUpdateData.Meshes)
		{
			if(PartReplacement.BoneName.IsNone() || !PartReplacement.StaticMesh)
				continue;

			const USkeletalMeshSocket* SocketForAttachment = GetMesh()->GetSocketByName(PartReplacement.BoneName);

			if (SocketForAttachment)
			{
				// TODO: Implement replacement
				//SocketForAttachment->AttachActor(this, PartReplacement.StaticMesh);
				//GetMesh()->AttachToComponent()
			}
		}
	}
}

bool AMRMech::GetReplacementPart(const EMechPart MechPart, const EHealthState HealthState, FMechPartUpdateData& OutMechPartReplacements)
{
	if (HealthState == EHealthState::EHS_Damaged)
	{
		if (MechModelData.MechDestructibleState.DamageReplacements.Contains(MechPart))
		{
			OutMechPartReplacements = MechModelData.MechDestructibleState.DamageReplacements[MechPart];
			return true;
		}
	} else if (HealthState == EHealthState::EHS_Destroyed)
	{
		if (MechModelData.MechDestructibleState.DestroyReplacements.Contains(MechPart))
		{
			OutMechPartReplacements = MechModelData.MechDestructibleState.DestroyReplacements[MechPart];
			return true;
		}
	}

	return false;
}

bool AMRMech::GetBonesToHide(const EMechPart MechPart, const EHealthState HealthState, FMechBonesToHide& OutMechBonesToHide)
{
	UMRMechDestructionHierarchyAsset* HierarchyAsset = MechModelData.MechDestructibleState.DestructionHierarchyAsset;
	if (!HierarchyAsset)
		return false;
	
	if (HealthState == EHealthState::EHS_Damaged)
	{
		if (HierarchyAsset->DamagedPartsToHide.Contains(MechPart))
		{
			OutMechBonesToHide = HierarchyAsset->DamagedPartsToHide[MechPart];
			return true;
		}
	} else if (HealthState == EHealthState::EHS_Destroyed)
	{
		if (HierarchyAsset->DestroyedPartsToHide.Contains(MechPart))
		{
			OutMechBonesToHide = HierarchyAsset->DestroyedPartsToHide[MechPart];
			return true;
		}
	}

	return false;
}

bool AMRMech::GetMechPartUpdateData(const EMechPart MechPart, const EHealthState HealthState, FMechPartUpdateData& OutMechPartUpdateData)
{
	if (HealthState == EHealthState::EHS_Damaged)
	{
		if (MechModelData.MechDestructibleState.DamageReplacements.Contains(MechPart))
		{
			OutMechPartUpdateData = MechModelData.MechDestructibleState.DamageReplacements[MechPart];
			return true;
		}
	} else if (HealthState == EHealthState::EHS_Destroyed)
	{
		if (MechModelData.MechDestructibleState.DestroyReplacements.Contains(MechPart))
		{
			OutMechPartUpdateData = MechModelData.MechDestructibleState.DestroyReplacements[MechPart];
			return true;
		}
	}

	return false;
}

void AMRMech::SpawnParticle(FMechParticleSpawnData& ParticleData)
{
	if (ParticleData.ParticleSystem && !ParticleData.BoneName.IsNone())
	{
		UGameplayStatics::SpawnEmitterAttached(ParticleData.ParticleSystem, GetMesh(), ParticleData.BoneName);
	}
}

void AMRMech::SpawnSound(FMechSoundSpawnData& SoundData)
{
	if (SoundData.Audio && !SoundData.BoneName.IsNone())
	{
		UGameplayStatics::SpawnSoundAttached(SoundData.Audio, GetMesh(), SoundData.BoneName);
	}
}

void AMRMech::Death()
{
	ULog::Success("AMRMech::Death", LO_Both);
	WeaponSystemComponent->StopAllWeaponsFire();
	GetMovementComponent()->StopMovementImmediately();
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
}

void AMRMech::DestroyPart(const EMechPart MechPart)
{
	DestroyPart(MechPart, EHealthState::EHS_Destroyed);
}

void AMRMech::DamagePart(const EMechPart MechPart)
{
	// TODO: Implement
}
