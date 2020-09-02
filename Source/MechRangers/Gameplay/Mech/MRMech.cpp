// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMech.h"
#include "Components/CapsuleComponent.h"
#include "MechComponents/MRMechMovementComponent.h"
#include "MechComponents/MRMechLivingComponent.h"
#include "MechDataAssets/MRMechLoadoutDataAsset.h"
#include "MechDataAssets/MRMechModelDataAsset.h"
#include "MRMechAnimInstance.h"
#include "MRMechCockpit.h"

// Sets default values
AMRMech::AMRMech(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UMRMechMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Defaults
	LivingComponent = CreateDefaultSubobject<UMRMechLivingComponent>(TEXT("LivingComponent"));
}

// Called when the game starts or when spawned
void AMRMech::BeginPlay()
{
	Super::BeginPlay();

	check(MechLoadoutAsset);
	
	MechLoadout = MechLoadoutAsset->GetLoadout();
	MechModelData = MechLoadout.MechModelAsset->GetModelData();

	SetupMech();
}

// Called every frame
void AMRMech::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMRMech::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMRMech::MoveForward);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMRMech::TurnAtRate);
}

void AMRMech::SetupMech()
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
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate = 20.f;
	
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Val * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


