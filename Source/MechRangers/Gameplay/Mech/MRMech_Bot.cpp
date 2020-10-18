// Copyright PlatoSpace.com All Rights Reserved.


#include "MRMech_Bot.h"
#include "Log.h"
#include "MRMechAIController.h"
#include "Components/SphereComponent.h"
#include "MechRangers/Gameplay/Mech/MRMechAim.h"

AMRMech_Bot::AMRMech_Bot(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AIControllerClass = AMRMechAIController::StaticClass();
    
    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(RootComponent);
    CombatSphere->SetSphereRadius(2000.f);
    CombatSphere->SetCollisionProfileName(FName("Trigger"));

    bAttacking = false;
    bFiring = false;
}

void AMRMech_Bot::BeginPlay()
{
    Super::BeginPlay();

    CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMRMech_Bot::CombatSphereOnOverlapBegin);
    CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMRMech_Bot::CombatSphereOnOverlapEnd);
    bIsCombatMode = true;
}

void AMRMech_Bot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMRMech_Bot::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // we have already a target
    if (bAttacking && CombatTarget)
        return;

    if (IsAbleAttack(OtherActor) && Alive())
    {
        bOverlappingCombatSphere = true;
        AttackTarget(OtherActor);
    }
}

void AMRMech_Bot::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    StopAttack();
    ClearCombatTarget();

    // Try to find another target
    AActor* NewTarget = FindTarget();
    if (NewTarget)
    {
        AttackTarget(NewTarget);
    }
}

bool AMRMech_Bot::IsAbleAttack(AActor* InActor) const
{
    if (!InActor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
        return false;

    IMRDamageTakerInterface* NewTarget = Cast<IMRDamageTakerInterface>(InActor);
    if (!NewTarget)
        return false;

    /** Only actors which implements interface and from different gameplay team */
    return NewTarget->Alive() && NewTarget->GetGameplayTeam() != GetGameplayTeam();
}

void AMRMech_Bot::AttackTarget(AActor* Target)
{
    if (!Alive() || !IsValid(Target) || !IsAbleAttack(Cast<AActor>(Target)))
        return;

    if (Target->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
    {
        // Subscribe on death
        Cast<IMRDamageTakerInterface>(Target)->OnDeath().AddUObject(this, &AMRMech_Bot::OnTargetDeath);
    }

    bAttacking = true;
    CombatTarget = Target;

    StartWeaponFire();

    // set timer for looking better target
    if (TimerHandle_HandleBetterTarget.IsValid() == false)
    {
        GetWorldTimerManager().SetTimer(TimerHandle_HandleBetterTarget, this, &AMRMech_Bot::HandleBetterTarget, 3.f, true);
    }
}

void AMRMech_Bot::AttackFinish()
{
    StopAttack();
    ClearCombatTarget();
	
    if (bOverlappingCombatSphere && CombatTarget)
    {
        AttackTarget(CombatTarget);
    }
}

void AMRMech_Bot::StopAttack()
{
    bAttacking = false;
    //StopWeaponFire();

    //AimingDeactivate();

    if (TimerHandle_HandleBetterTarget.IsValid())
    {
        TimerHandle_HandleBetterTarget.Invalidate();
    }
}

void AMRMech_Bot::OnTargetDeath(AActor* DeadActor)
{
    //StopAttack();
    ClearCombatTarget();
	
    // Try to find another target
    AActor* NewTarget = FindTarget();
    if (NewTarget)
    {
        // ULog::Success("Found another Target!", LO_Both);
        AttackTarget(NewTarget);
    }
}

AActor* AMRMech_Bot::FindTarget() const
{
    if (!Alive())
        return nullptr;
	
    TArray<AActor*> OverlappingActors;
    CombatSphere->GetOverlappingActors(OverlappingActors);
    if (OverlappingActors.Num() <= 0)
        return nullptr;
	
    for (AActor* OverlappingActor : OverlappingActors)
    {
        if (OverlappingActor != CombatTarget && IsAbleAttack(OverlappingActor))
        {
            return OverlappingActor;
        }
    }

    return nullptr;
}

void AMRMech_Bot::StartWeaponFire()
{
        
    if (bFiring || !WeaponSystemComponent->CanFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Primary))
        return;

    bFiring = true;

    // set timer for check stop firing
    GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AMRMech_Bot::HandleStopFiring, 1.5f, true);

    WeaponSystemComponent->StartWeaponFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Primary);
    WeaponSystemComponent->StartWeaponFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Primary);
}

void AMRMech_Bot::StopWeaponFire()
{
    if (!bFiring)
        return;

    bFiring = false;
    WeaponSystemComponent->StopWeaponFire(EMechPart::EMP_LeftArm, EWeaponGroup::EWG_Primary);
    WeaponSystemComponent->StopWeaponFire(EMechPart::EMP_RightArm, EWeaponGroup::EWG_Primary);

    if (TimerHandle_HandleFiring.IsValid())
    {
        TimerHandle_HandleFiring.Invalidate();
    }
}

void AMRMech_Bot::HandleStopFiring()
{
    if (bFiring && !CombatTarget)
    {
        StopWeaponFire();
    }
}

void AMRMech_Bot::HandleBetterTarget()
{
    // Try to attack damage causer first
    if (LastAttacker && IsValid(LastAttacker) && LastAttacker != CombatTarget)
    {
        AttackTarget(LastAttacker);
    }

    LastAttacker = nullptr;
}

void AMRMech_Bot::ClearCombatTarget()
{
    if (IsValid(CombatTarget) && CombatTarget->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
    {
        Cast<IMRDamageTakerInterface>(CombatTarget)->OnDeath().RemoveAll(this);
    }
    CombatTarget = nullptr;
}
