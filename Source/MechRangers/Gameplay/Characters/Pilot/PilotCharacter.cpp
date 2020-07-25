// Fill out your copyright notice in the Description page of Project Settings.


#include "PilotCharacter.h"
#include "Log.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "MechRangers/Core/MechRangersGameMode.h"
#include "MechRangers/Gameplay/Characters/Animations/HandAnimInstance.h"
#include "MechRangers/Gameplay/Mechs/BaseMech.h"

APilotCharacter::APilotCharacter()
{
    bVRMode = true;

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

    FlatCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FlatCamera"));
    FlatCamera->SetupAttachment(GetRootComponent());
    FlatCamera->SetAutoActivate(false);

    // VRReplicatedCamera->SetAutoActivate(false);
}

void APilotCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Set OwnedPilot to Mech
    if (OwningMech)
    {
        OwningMech->PilotSitDown(this);
    }

    UWorld* World = GetWorld();
    if (World)
    {
        AMechRangersGameMode* GameMode = Cast<AMechRangersGameMode>(World->GetAuthGameMode());
    
        bVRMode = GameMode->bVRMode;
    }
    
    // Set Tracking to Floor. Without it in Oculus Rift is under floor.
    if (bVRMode)
    {
        VRReplicatedCamera->SetAutoActivate(true);
        UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
    } else
    {
        FlatCamera->SetAutoActivate(true);
    }

    

}

void APilotCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckAndHandleGripAnimations();
}

void APilotCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    // Grip Actions
    PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &APilotCharacter::GripLeftPressed);
    PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &APilotCharacter::GripLeftReleased);
    PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &APilotCharacter::GripRightPressed);
    PlayerInputComponent->BindAction("GripRight", IE_Released, this, &APilotCharacter::GripRightReleased);
}

ABaseMech* APilotCharacter::GetOwningMech() const
{
    return OwningMech;
}

void APilotCharacter::GripLeftPressed()
{
    bGripPressedLeft = true;

    CheckAndHandleGrip(GrabSphereLeft, LeftMotionController);
}

void APilotCharacter::GripRightPressed()
{
    bGripPressedRight = true;

    CheckAndHandleGrip(GrabSphereRight, RightMotionController);
}

void APilotCharacter::GripLeftReleased()
{
    bGripPressedLeft = false;

    DropAllGrips(LeftMotionController);
}

void APilotCharacter::GripRightReleased()
{
    bGripPressedRight = false;

    DropAllGrips(RightMotionController);
}

//----------------------------------------------------------------------------------------------------------------------
// Hands
//----------------------------------------------------------------------------------------------------------------------

bool APilotCharacter::TraceFromController(UGripMotionControllerComponent* CallingController,
    FHitResult& OutHitResult) const
{
    const float MaxTraceDistance = 500.f;

    const FVector TraceStart = CallingController->GetComponentLocation();
    const FVector TraceEnd = TraceStart + (CallingController->GetForwardVector() * MaxTraceDistance);

    const TArray<AActor*> ActorsToIgnore;
    return UKismetSystemLibrary::SphereTraceSingle(GetWorld(), TraceStart, TraceEnd, 5.f, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true, FLinearColor::Green, FLinearColor::Red, 0.3f);
}

UPrimitiveComponent* APilotCharacter::GetNearestOverlappingObject(UPrimitiveComponent* OverlapComponent, const FName Tag)
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
    ActorsToIgnore.Add(OwningMech);

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

void APilotCharacter::CheckAndHandleGrip(UPrimitiveComponent* GrabSphere, UGripMotionControllerComponent* CallingController)
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

void APilotCharacter::DropAllGrips(UGripMotionControllerComponent* CallingController) const
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

void APilotCharacter::GripDropped(UGripMotionControllerComponent* GripController, UObject* DroppedObject)
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

void APilotCharacter::CheckAndHandleGripAnimations()
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

void APilotCharacter::CheckAndHandleGripControllerAnimations(UPrimitiveComponent* GrabSphere,
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

bool APilotCharacter::HasValidGripCollision(UPrimitiveComponent* Component)
{
    // Get Collision response for VRTraceChannel
    const ECollisionResponse CollisionResponse = Component->GetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1);

    switch (CollisionResponse)
    {
    case ECR_Overlap:
    case ECR_Block:
        return true;
    default:
        return false;
    }
}


