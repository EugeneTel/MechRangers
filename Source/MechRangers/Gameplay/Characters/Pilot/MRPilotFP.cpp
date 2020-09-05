// Copyright PlatoSpace.com All Rights Reserved.

#include "MRPilotFP.h"
#include "PilotComponents/MRMechControlComponent.h"
#include "MechRangers/Gameplay/Mech/MRMechCockpit.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Log.h"

// Sets default values
AMRPilotFP::AMRPilotFP()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create Components
	MechControl = CreateDefaultSubobject<UMRMechControlComponent>(TEXT("MechControlComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());

	// Setup configs
	AutoPossessAI = EAutoPossessAI::Disabled;
	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	bUseControllerRotationPitch = true;

	// Setup defaults
	bIsMovementMode = true;
}

// Called when the game starts or when spawned
void AMRPilotFP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AMRPilotFP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &AMRPilotFP::MechMoveForward);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMRPilotFP::MechTurnAtRate);
	PlayerInputComponent->BindAxis("Move_LimbLeftY", this, &AMRPilotFP::Move_LimbLeftY);
	PlayerInputComponent->BindAxis("Turn_LimbLeftX", this, &AMRPilotFP::Turn_LimbLeftX);
	PlayerInputComponent->BindAxis("LookRight_LimbRightX", this, &AMRPilotFP::LookRight_LimbRightX);
	PlayerInputComponent->BindAxis("LookUp_LimbRightY", this, &AMRPilotFP::LookUp_LimbRightY);

	// Action
	PlayerInputComponent->BindAction("ChangeMovementMode", EInputEvent::IE_Pressed, this, &AMRPilotFP::ChangeMovementModePressed);

}

//----------------------------------------------------------------------------------------------------------------------
// Mech Control Wrapper
//----------------------------------------------------------------------------------------------------------------------

UMRMechControlComponent* AMRPilotFP::GetMechControlComponent() const
{
	return MechControl;
}

void AMRPilotFP::SitIntoMech_Implementation(AMRMech* NewMech)
{
	if (NewMech->Cockpit)
	{
		AttachToComponent(NewMech->Cockpit->GetPilotAttachmentPoint(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	} else
	{
		UE_LOG(LogActor, Warning, TEXT("SitIntoMech: Can't sit a Pilot into a Mech. Cockpit is not set!"));
	}

	MechControl->SitPilotIntoMech(NewMech);
}

void AMRPilotFP::MechMoveForward(float Val)
{
	if (Val != 0)
	{
		MechControl->MoveForward(Val);
	}
}

void AMRPilotFP::MechTurnAtRate(float Val)
{
	if (Val != 0)
	{
		MechControl->TurnAtRate(Val);
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Pilot control
//----------------------------------------------------------------------------------------------------------------------

void AMRPilotFP::LookUp(float Val)
{
	if (Val == 0)
		return;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseLookUpRate = 100.f;
	float Delta = Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds();

	CameraComponent->AddRelativeRotation(FRotator(Delta, 0.f, 0.f));

	// Limit camera rotation
	const float LookLimit = 60.f;
	FRotator CameraRotation = CameraComponent->GetRelativeRotation();
	if (FMath::Abs(CameraRotation.Pitch) > LookLimit)
	{
		CameraRotation.Pitch = CameraRotation.Pitch > 0 ? LookLimit : LookLimit * -1;
		CameraComponent->SetRelativeRotation(CameraRotation);
	}
}

void AMRPilotFP::LookRight(float Val)
{
	if (Val == 0)
		return;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	float BaseTurnRate = 100.f;
	float Delta = Val * BaseTurnRate * GetWorld()->GetDeltaSeconds();
	
	CameraComponent->AddRelativeRotation(FRotator(0.f, Delta, 0.f));

	// Limit camera rotation
	const float LookLimit = 70.f;
	FRotator CameraRotation = CameraComponent->GetRelativeRotation();
	if (FMath::Abs(CameraRotation.Yaw) > LookLimit)
	{
		CameraRotation.Yaw = CameraRotation.Yaw > 0 ? LookLimit : LookLimit * -1;
		CameraComponent->SetRelativeRotation(CameraRotation);
	}
	
}

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------

void AMRPilotFP::Move_LimbLeftY(float Val)
{
	if (Val == 0.f)
		return;
	
	if (bIsMovementMode)
	{
		MechControl->MoveForward(Val);
	} else
	{
		// TODO: Limb control
	}
}

void AMRPilotFP::Turn_LimbLeftX(float Val)
{
	if (Val == 0.f)
		return;
	
	if (bIsMovementMode)
	{
		MechControl->TurnAtRate(Val);
	} else
	{
		// TODO: Limb control
	}
}

void AMRPilotFP::LookRight_LimbRightX(float Val)
{
	if (bIsMovementMode)
	{
		LookRight(Val);
	} else
	{
		// TODO: Limb control
	}
}

void AMRPilotFP::LookUp_LimbRightY(float Val)
{
	if (bIsMovementMode)
	{
		LookUp(Val);
	} else
	{
		// TODO: Limb control
	}
}

void AMRPilotFP::ChangeMovementModePressed()
{
	bIsMovementMode = !bIsMovementMode;
	ULog::Bool(bIsMovementMode, "Movement Mode:", "", LO_Both);
}
