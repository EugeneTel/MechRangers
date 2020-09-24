// Fill out your copyright notice in the Description page of Project Settings.


#include "MRPilotHandsVR.h"
#include "Log.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "MechRangers/MechRangers.h"
#include "MechRangers/Core/MechRangersGameMode.h"
#include "MechRangers/Gameplay/Characters/Animations/HandAnimInstance.h"
#include "MechRangers/Gameplay/Mech/MRMech.h"
#include "MechRangers/Gameplay/Mech/MRMechCockpit.h"
#include "PilotComponents/MRMechControlComponent.h"

AMRPilotHandsVR::AMRPilotHandsVR()
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

void AMRPilotHandsVR::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (World)
    {
        AMechRangersGameMode* GameMode = Cast<AMechRangersGameMode>(World->GetAuthGameMode());
    }
    
    // Set Tracking to Floor. Without it in Oculus Rift is under floor.
    UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
}

void AMRPilotHandsVR::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckAndHandleGripAnimations();
}

void AMRPilotHandsVR::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    // Input Axis
    PlayerInputComponent->BindAxis("MoveForward", this, &AMRPilotHandsVR::MechMoveForward);
    PlayerInputComponent->BindAxis("TurnRate", this, &AMRPilotHandsVR::MechTurnAtRate);

    // Grip Actions
    PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &AMRPilotHandsVR::GripLeftPressed);
    PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &AMRPilotHandsVR::GripLeftReleased);
    PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &AMRPilotHandsVR::GripRightPressed);
    PlayerInputComponent->BindAction("GripRight", IE_Released, this, &AMRPilotHandsVR::GripRightReleased);
}

void AMRPilotHandsVR::SitIntoMech_Implementation(AMRMech* NewMech)
{
    if (NewMech->Cockpit)
    {
        // AttachToComponent(NewMech->Cockpit->GetPilotAttachmentPoint(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

        const FTransform RelativeTransformToSeat = FTransform(FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector,FVector::OneVector);
        SetSeatedMode(NewMech->Cockpit->GetPilotAttachmentPoint(), true, RelativeTransformToSeat, VRReplicatedCamera->GetRelativeTransform());
        MechControl->SitPilotIntoMech(NewMech);
        NewMech->SetOwner(this);
    }
    else
    {
        UE_LOG(LogActor, Warning, TEXT("SitIntoMech: Can't sit a Pilot into a Mech. Cockpit is not set!"));
    }

  
}

AMRMech* AMRPilotHandsVR::GetMech() const
{
    return Mech;
}

void AMRPilotHandsVR::GripLeftPressed()
{
    bGripPressedLeft = true;

    CheckAndHandleGrip(GrabSphereLeft, LeftMotionController);
}

void AMRPilotHandsVR::GripRightPressed()
{
    bGripPressedRight = true;

    CheckAndHandleGrip(GrabSphereRight, RightMotionController);
}

void AMRPilotHandsVR::GripLeftReleased()
{
    bGripPressedLeft = false;

    DropAllGrips(LeftMotionController);
}

void AMRPilotHandsVR::GripRightReleased()
{
    bGripPressedRight = false;

    DropAllGrips(RightMotionController);
}

void AMRPilotHandsVR::MechMoveForward(const float Val)
{
    if (Val != 0)
    {
        MechControl->MoveForward(Val);
    }
}

void AMRPilotHandsVR::MechTurnAtRate(const float Val)
{
    if (Val != 0)
    {
        MechControl->TurnAtRate(Val);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Hands
//----------------------------------------------------------------------------------------------------------------------

bool AMRPilotHandsVR::TraceFromController(UGripMotionControllerComponent* CallingController,
    FHitResult& OutHitResult) const
{
    const float MaxTraceDistance = 500.f;

    const FVector TraceStart = CallingController->GetComponentLocation();
    const FVector TraceEnd = TraceStart + (CallingController->GetForwardVector() * MaxTraceDistance);

    const TArray<AActor*> ActorsToIgnore;
    return UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 5.f, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true, FLinearColor::Green, FLinearColor::Red, 0.3f);
}

UPrimitiveComponent* AMRPilotHandsVR::GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent, const FName Tag)
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

void AMRPilotHandsVR::CheckAndHandleGrip(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController)
{
    UPrimitiveComponent* NearestObject = GetNearestOverlappingObject(GrabSphere);

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

void AMRPilotHandsVR::DropAllGrips(UGripMotionControllerComponent* CallingController) const
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

void AMRPilotHandsVR::GripDropped(UGripMotionControllerComponent* GripController, UObject* DroppedObject)
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

void AMRPilotHandsVR::CheckAndHandleGripAnimations()
{
    // Left Hand
    CheckAndHandleGripControllerAnimations(GrabSphereLeft, LeftMotionController, bGripPressedLeft, GripStateLeft);
    UHandAnimInstance* LeftAnimInst = Cast<UHandAnimInstance>(HandMeshLeft->GetAnimInstance());
    LeftAnimInst->GripState = GripStateLeft;
	
    // Right Hand
    CheckAndHandleGripControllerAnimations(GrabSphereRight, RightMotionController, bGripPressedRight, GripStateRight);
    UHandAnimInstance* RightAnimInst = Cast<UHandAnimInstance>(HandMeshRight->GetAnimInstance());
    RightAnimInst->GripState = GripStateRight;
}

void AMRPilotHandsVR::CheckAndHandleGripControllerAnimations(UPrimitiveComponent* GrabSphere,
    UGripMotionControllerComponent* CallingController, const bool bGripPressed, EGripState& GripState)
{
    if (bGripPressed || CallingController->HasGrippedObjects())
    {
        GripState = EGripState::EGS_Grab;
    } else
    {
        UPrimitiveComponent* NearestObject = GetNearestOverlappingObject(GrabSphere);

        if (IsValid(NearestObject))
        {
            GripState = EGripState::EGS_CanGrab;
        } else
        {
            GripState = EGripState::EGS_Open;
        }
    }
}

bool AMRPilotHandsVR::HasValidGripCollision(UPrimitiveComponent* Component)
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


