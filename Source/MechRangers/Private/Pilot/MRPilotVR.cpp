// Fill out your copyright notice in the Description page of Project Settings.


#include "Pilot/MRPilotVR.h"
#include "Log.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "MechRangers/MechRangers.h"
#include "MechRangers/Modes/MRGameMode.h"
#include "Pilot/MRSingleHandAnimInstance.h"
#include "Mech/MRMech.h"
#include "Mech/Cockpit/MRMechCockpit.h"
#include "Mech/Cockpit/MRMechCockpitPilot.h"
#include "Pilot/MRMechControlComponent.h"

AMRPilotVR::AMRPilotVR()
{
    /**
    * Hands components
    */
    HandMeshRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMeshRight"));
    HandMeshRight->SetupAttachment(RightMotionController);

    HandMeshLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMeshLeft"));
    HandMeshLeft->SetupAttachment(LeftMotionController);

    GrabSphereRight = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereRight"));
    GrabSphereRight->SetupAttachment(RightMotionController);
    GrabSphereRight->SetSphereRadius(4.f);
    // TODO: Prepare collision presets for Sphere

    GrabSphereLeft = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphereLeft"));
    GrabSphereLeft->SetupAttachment(LeftMotionController);
    GrabSphereLeft->SetSphereRadius(4.f);
    // TODO: Prepare collision presets for Sphere

    MechControl = CreateDefaultSubobject<UMRMechControlComponent>(TEXT("MechControlComponent"));

    // Configs
    VRRootReference->SetCollisionProfileName(FName("NoCollision"));
    VRRootReference->SetGenerateOverlapEvents(false);
    GetMesh()->SetCollisionProfileName(FName("NoCollision"));
    GrabSphereLeft->SetCollisionProfileName(FName("VROnly"));
    GrabSphereRight->SetCollisionProfileName(FName("VROnly"));
}

void AMRPilotVR::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (World)
    {
        AMRGameMode* GameMode = Cast<AMRGameMode>(World->GetAuthGameMode());
    }
    
    // Set Tracking to Floor. Without it in Oculus Rift is under floor.
    UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);

    // Reset VR orientation and position
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(0, EOrientPositionSelector::OrientationAndPosition);
}

void AMRPilotVR::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckAndHandleGripAnimations();
}

void AMRPilotVR::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    // Input Axis
    PlayerInputComponent->BindAxis("MoveForward", this, &AMRPilotVR::OnMechMoveForward);
    PlayerInputComponent->BindAxis("TurnRate", this, &AMRPilotVR::OnMechTurnAtRate);
    PlayerInputComponent->BindAxis("GripLeftAxis", this, &AMRPilotVR::OnGripLeftAxis);
    PlayerInputComponent->BindAxis("GripRightAxis", this, &AMRPilotVR::OnGripRightAxis);

    // Grip Actions
    PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &AMRPilotVR::OnGripLeftPressed);
    PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &AMRPilotVR::OnGripLeftReleased);
    PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &AMRPilotVR::OnGripRightPressed);
    PlayerInputComponent->BindAction("GripRight", IE_Released, this, &AMRPilotVR::OnGripRightReleased);
    
    PlayerInputComponent->BindAction("PrimaryLeftAction", EInputEvent::IE_Pressed, this, &AMRPilotVR::OnPrimaryLeftActionPressed);
    PlayerInputComponent->BindAction("PrimaryLeftAction", EInputEvent::IE_Released, this, &AMRPilotVR::OnPrimaryLeftActionReleased);
    PlayerInputComponent->BindAction("PrimaryRightAction", EInputEvent::IE_Pressed, this, &AMRPilotVR::OnPrimaryRightActionPressed);
    PlayerInputComponent->BindAction("PrimaryRightAction", EInputEvent::IE_Released, this,  &AMRPilotVR::OnPrimaryRightActionReleased);
    
    PlayerInputComponent->BindAction("SecondaryLeftAction", EInputEvent::IE_Pressed, this, &AMRPilotVR::OnSecondaryLeftActionPressed);
    PlayerInputComponent->BindAction("SecondaryLeftAction", EInputEvent::IE_Released, this, &AMRPilotVR::OnSecondaryLeftActionReleased);
    PlayerInputComponent->BindAction("SecondaryRightAction", EInputEvent::IE_Pressed, this, &AMRPilotVR::OnSecondaryRightActionPressed);
    PlayerInputComponent->BindAction("SecondaryRightAction", EInputEvent::IE_Released, this, &AMRPilotVR::OnSecondaryRightActionReleased);
}

void AMRPilotVR::SitIntoMech_Implementation(AMRMech* NewMech)
{
    if (NewMech->Cockpit)
    {
        if (const auto CockpitPilot = Cast<AMRMechCockpitPilot>(NewMech->Cockpit))
        {
            // AttachToComponent(NewMech->Cockpit->GetPilotAttachmentPoint(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

            const FTransform RelativeTransformToSeat = FTransform(FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector,FVector::OneVector);
            SetSeatedMode(CockpitPilot->GetPilotAttachmentPoint(), true, RelativeTransformToSeat, VRReplicatedCamera->GetRelativeTransform());
            MechControl->SitPilotIntoMech(NewMech);
            NewMech->SetOwner(this);
            Mech = NewMech;
        }
    }
    else
    {
        UE_LOG(LogActor, Warning, TEXT("SitIntoMech: Can't sit a Pilot into a Mech. Cockpit is not set!"));
    }

  
}

AMRMech* AMRPilotVR::GetMech() const
{
    return Mech;
}

void AMRPilotVR::OnGripLeftPressed()
{
    bGripPressedLeft = true;

    CheckAndHandleGrip(GrabSphereLeft, LeftMotionController);
}

void AMRPilotVR::OnGripRightPressed()
{
    bGripPressedRight = true;

    CheckAndHandleGrip(GrabSphereRight, RightMotionController);
}

void AMRPilotVR::OnGripLeftReleased()
{
    bGripPressedLeft = false;

    DropAllGrips(LeftMotionController);
}

void AMRPilotVR::OnGripRightReleased()
{
    bGripPressedRight = false;

    DropAllGrips(RightMotionController);
}

void AMRPilotVR::OnPrimaryLeftActionPressed()
{
    if (MechControl->IsManipulatorLeftHeld())
    {
        MechControl->PrimaryLeftWeaponStart();
    }
}

void AMRPilotVR::OnPrimaryLeftActionReleased()
{
    if (MechControl->IsManipulatorLeftHeld())
    {
        MechControl->PrimaryLeftWeaponFinish();
    }
}

void AMRPilotVR::OnPrimaryRightActionPressed()
{
    if (MechControl->IsManipulatorRightHeld())
    {
        MechControl->PrimaryRightWeaponStart();
    }
}

void AMRPilotVR::OnPrimaryRightActionReleased()
{
    if (MechControl->IsManipulatorRightHeld())
    {
        MechControl->PrimaryRightWeaponFinish();
    }
}

void AMRPilotVR::OnSecondaryLeftActionPressed()
{
    if (MechControl->IsManipulatorLeftHeld())
    {
        MechControl->SecondaryLeftWeaponStart();
    }
}

void AMRPilotVR::OnSecondaryLeftActionReleased()
{
    if (MechControl->IsManipulatorLeftHeld())
    {
        MechControl->SecondaryLeftWeaponFinish();
    }
}

void AMRPilotVR::OnSecondaryRightActionPressed()
{
    if (MechControl->IsManipulatorRightHeld())
    {
        MechControl->SecondaryRightWeaponStart();
    }
}

void AMRPilotVR::OnSecondaryRightActionReleased()
{
    if (MechControl->IsManipulatorRightHeld())
    {
        MechControl->SecondaryRightWeaponFinish();
    }
}

void AMRPilotVR::OnGripLeftAxis(const float Val)
{

    GripLeftAxis = Val;
}

void AMRPilotVR::OnGripRightAxis(const float Val)
{
    GripRightAxis = Val;
}

void AMRPilotVR::OnMechMoveForward(const float Val)
{
    if (Val != 0)
    {
        MechControl->MoveForward(Val);
    }
}

void AMRPilotVR::OnMechTurnAtRate(const float Val)
{
    if (Val != 0)
    {
        MechControl->TurnAtRate(Val);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Hands
//----------------------------------------------------------------------------------------------------------------------

bool AMRPilotVR::TraceFromController(UGripMotionControllerComponent* CallingController,
    FHitResult& OutHitResult) const
{
    const float MaxTraceDistance = 500.f;

    const FVector TraceStart = CallingController->GetComponentLocation();
    const FVector TraceEnd = TraceStart + (CallingController->GetForwardVector() * MaxTraceDistance);

    const TArray<AActor*> ActorsToIgnore;
    return UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 5.f, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true, FLinearColor::Green, FLinearColor::Red, 0.3f);
}

UPrimitiveComponent* AMRPilotVR::GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent, const FName Tag)
{
    if (OverlapComponent == nullptr)
    {
        return nullptr;
    }
	
    // Prepare parameters for ComponentOverlapComponents

    // set collision presets (WorldStatic, WorldDynamic etc.)
    TArray < TEnumAsByte< EObjectTypeQuery >> ComponentTransform;
    ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery1);
    ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery2);
    ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery3);
    ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery4);
    ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery5);
    ComponentTransform.Add(EObjectTypeQuery::ObjectTypeQuery6);

    // add self to ignore list on collision
    TArray <AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    ActorsToIgnore.Add(Mech);

    // prepare variable for results
    TArray <UPrimitiveComponent*> OutComponents;

    // get overlapping components
    if (!UKismetSystemLibrary::ComponentOverlapComponents(OverlapComponent, OverlapComponent->GetComponentTransform(), ComponentTransform, nullptr, ActorsToIgnore, OutComponents))
    {
        return nullptr;
    }

    // Go through all overlapped components and looking for the first valid
    for (UPrimitiveComponent* OutComponent : OutComponents)
    {
        const bool bByTag = Tag == "" ? true : OutComponent->ComponentHasTag(Tag);
        if (HasValidGripCollision(OutComponent) && bByTag)
        {
            return OutComponent;
        }
    }

    return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
// Gripping
//----------------------------------------------------------------------------------------------------------------------

void AMRPilotVR::CheckAndHandleGrip(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController)
{
    UPrimitiveComponent* NearestObject = GetNearestOverlappingObject(GrabSphere, FName("Grippable"));

    if (!IsValid(NearestObject))
        return;

    if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(NearestObject))
    {
        if (PrimComp->GetClass()->ImplementsInterface(UVRGripInterface::StaticClass()))
        {
            CallingController->GripObjectByInterface(PrimComp, PrimComp->GetComponentTransform());
        }
    }
    else
    {
        if (AActor* Actor = Cast<AActor>(NearestObject))
        {
            if (Actor->GetClass()->ImplementsInterface(UVRGripInterface::StaticClass()))
            {
                CallingController->GripObjectByInterface(Actor, Actor->GetActorTransform());
            }
        }
    }
}

void AMRPilotVR::DropAllGrips(UGripMotionControllerComponent* CallingController) const
{
    if (!CallingController->HasGrippedObjects())
        return;

    TArray<UObject*> GrippedObjectsArray; 

    CallingController->GetGrippedObjects(GrippedObjectsArray);
	
    for (UObject* GrippedObject : GrippedObjectsArray)
    {
        // Drop all objects even if not implements interface
        if (CallingController->DropObjectByInterface(GrippedObject))
        {
            OnGripDropped.Broadcast(CallingController, GrippedObject);
        }
    }
}

void AMRPilotVR::GripDropped(UGripMotionControllerComponent* GripController, UObject* DroppedObject)
{
    AActor* DroppedActor = Cast<AActor>(DroppedObject);
    if (!DroppedActor)
    {
        if (UPrimitiveComponent * PrimComp = Cast<UPrimitiveComponent>(DroppedObject))
        {
            DroppedActor = PrimComp->GetOwner();
        }
    }

    if (!DroppedActor)
        return;
	

    // TODO: Do something with dropped actor
}

void AMRPilotVR::CheckAndHandleGripAnimations()
{
    // Left Hand
    CheckAndHandleGripControllerAnimations(GrabSphereLeft, LeftMotionController, bGripPressedLeft, GripStateLeft);
    UMRSingleHandAnimInstance* LeftAnimInst = Cast<UMRSingleHandAnimInstance>(HandMeshLeft->GetAnimInstance());
    LeftAnimInst->GripState = GripStateLeft;
	
    // Right Hand
    CheckAndHandleGripControllerAnimations(GrabSphereRight, RightMotionController, bGripPressedRight, GripStateRight);
    UMRSingleHandAnimInstance* RightAnimInst = Cast<UMRSingleHandAnimInstance>(HandMeshRight->GetAnimInstance());
    RightAnimInst->GripState = GripStateRight;
}

void AMRPilotVR::CheckAndHandleGripControllerAnimations(UPrimitiveComponent* GrabSphere,
    UGripMotionControllerComponent* CallingController, const bool bGripPressed, EGripState& GripState)
{
    if (bGripPressed || CallingController->HasGrippedObjects())
    {
        GripState = EGripState::EGS_Grab;
    } else
    {
        UPrimitiveComponent* NearestObject = GetNearestOverlappingObject(GrabSphere, FName("Grippable"));

        if (IsValid(NearestObject))
        {
            GripState = EGripState::EGS_CanGrab;
        } else
        {
            GripState = EGripState::EGS_Open;
        }
    }
}

bool AMRPilotVR::HasValidGripCollision(UPrimitiveComponent* Component)
{
    // Get Collision response for VRTraceChannel
    const ECollisionResponse CollisionResponse = Component->GetCollisionResponseToChannel(COLLISION_VR_TRACE);

    switch (CollisionResponse)
    {
    case ECR_Overlap:
    case ECR_Block:
        return true;
    default:
        return false;
    }
}


