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
    bIsCombatMode = false;
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
    PlayerInputComponent->BindAxis("Move_ArmLeftYaw", this, &AMRPilotFP::Move_ArmLeftYaw);
    PlayerInputComponent->BindAxis("Turn_ArmLeftPitch", this, &AMRPilotFP::Turn_ArmLeftPitch);
    PlayerInputComponent->BindAxis("LookRight_ArmRightPitch", this, &AMRPilotFP::LookRight_ArmRightPitch);
    PlayerInputComponent->BindAxis("LookUp_ArmRightYaw", this, &AMRPilotFP::LookUp_ArmRightYaw);

    // Action
    PlayerInputComponent->BindAction("CombatMode", EInputEvent::IE_Pressed, this, &AMRPilotFP::CombatModePressed);
    PlayerInputComponent->BindAction("PrimaryLeftAction", EInputEvent::IE_Pressed, this,
                                     &AMRPilotFP::PrimaryLeftActionPressed);
    PlayerInputComponent->BindAction("PrimaryLeftAction", EInputEvent::IE_Released, this,
                                     &AMRPilotFP::PrimaryLeftActionReleased);
    PlayerInputComponent->BindAction("PrimaryRightAction", EInputEvent::IE_Pressed, this,
                                     &AMRPilotFP::PrimaryRightActionPressed);
    PlayerInputComponent->BindAction("PrimaryRightAction", EInputEvent::IE_Released, this,
                                     &AMRPilotFP::PrimaryRightActionReleased);
    PlayerInputComponent->BindAction("SecondaryLeftAction", EInputEvent::IE_Pressed, this,
                                     &AMRPilotFP::SecondaryLeftActionPressed);
    PlayerInputComponent->BindAction("SecondaryLeftAction", EInputEvent::IE_Released, this,
                                     &AMRPilotFP::SecondaryLeftActionReleased);
    PlayerInputComponent->BindAction("SecondaryRightAction", EInputEvent::IE_Pressed, this,
                                     &AMRPilotFP::SecondaryRightActionPressed);
    PlayerInputComponent->BindAction("SecondaryRightAction", EInputEvent::IE_Released, this,
                                     &AMRPilotFP::SecondaryRightActionReleased);
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
        AttachToComponent(NewMech->Cockpit->GetPilotAttachmentPoint(),
                          FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    }
    else
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
    const float BaseLookUpRate = 100.f;
    const float Delta = Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds();

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
        //CameraComponent->SetRelativeRotation(CameraRotation);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------

void AMRPilotFP::Move_ArmLeftYaw(float Val)
{
    if (Val == 0.f)
        return;

    if (bIsCombatMode)
    {
        MechControl->AddArmLeftYaw(Val);
    }
    else
    {
        MechControl->MoveForward(Val);
    }
}

void AMRPilotFP::Turn_ArmLeftPitch(float Val)
{
    if (Val == 0.f)
        return;

    if (bIsCombatMode)
    {
        MechControl->AddArmLeftPitch(Val);
    }
    else
    {
        MechControl->TurnAtRate(Val);
    }
}

void AMRPilotFP::LookRight_ArmRightPitch(float Val)
{
    if (Val == 0.f)
        return;

    if (bIsCombatMode)
    {
        MechControl->AddArmRightPitch(Val);
    }
    else
    {
        LookRight(Val);
    }
}

void AMRPilotFP::LookUp_ArmRightYaw(float Val)
{
    if (Val == 0.f)
        return;

    if (bIsCombatMode)
    {
        MechControl->AddArmRightYaw(Val);
    }
    else
    {
        LookUp(Val);
    }
}

void AMRPilotFP::CombatModePressed()
{
    bIsCombatMode = !bIsCombatMode;
    MechControl->SetCombatMode(bIsCombatMode);
    ULog::Bool(bIsCombatMode, "Set Combat Mode:", "", LO_Both);
}

void AMRPilotFP::PrimaryLeftActionPressed()
{
    if (bIsCombatMode)
    {
        MechControl->PrimaryLeftWeaponStart();
    }
}

void AMRPilotFP::PrimaryLeftActionReleased()
{
    if (bIsCombatMode)
    {
        MechControl->PrimaryLeftWeaponFinish();
    }
}

void AMRPilotFP::PrimaryRightActionPressed()
{
    if (bIsCombatMode)
    {
        MechControl->PrimaryRightWeaponStart();
    }
}

void AMRPilotFP::PrimaryRightActionReleased()
{
    if (bIsCombatMode)
    {
        MechControl->PrimaryRightWeaponFinish();
    }
}

void AMRPilotFP::SecondaryLeftActionPressed()
{
    if (bIsCombatMode)
    {
        MechControl->SecondaryLeftWeaponStart();
    }
}

void AMRPilotFP::SecondaryLeftActionReleased()
{
    if (bIsCombatMode)
    {
        MechControl->SecondaryLeftWeaponFinish();
    }
}

void AMRPilotFP::SecondaryRightActionPressed()
{
    if (bIsCombatMode)
    {
        MechControl->SecondaryRightWeaponStart();
    }
}

void AMRPilotFP::SecondaryRightActionReleased()
{
    if (bIsCombatMode)
    {
        MechControl->SecondaryRightWeaponFinish();
    }
}
