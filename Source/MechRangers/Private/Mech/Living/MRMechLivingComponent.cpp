// Copyright PlatoSpace.com All Rights Reserved.

#include "Mech/Living/MRMechLivingComponent.h"
#include "Log.h"
#include "DrawDebugHelpers.h"
#include "LivingSystem/MRHealthComponent.h"
#include "Mech/MRMech.h"

// Sets default values for this component's properties
UMRMechLivingComponent::UMRMechLivingComponent()
{
    // Configs
    PrimaryComponentTick.bCanEverTick = false;
    bDebug = true;
    MinNearDamageDistance = 100.f;
    MaxNearDamageDistance = 300.f;

    // Create health containers
    HealthContainerList.Add(EMechPart::EMP_Head, CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthContainer_Head")));
    HealthContainerList.Add(EMechPart::EMP_Torso, CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthContainer_Torso")));
    HealthContainerList.Add(EMechPart::EMP_RightArm, CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthContainer_RightArm")));
    HealthContainerList.Add(EMechPart::EMP_LeftArm, CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthContainer_LeftArm")));
    HealthContainerList.Add(EMechPart::EMP_RightLeg, CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthContainer_RightLeg")));
    HealthContainerList.Add(EMechPart::EMP_LeftLeg, CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthContainer_LeftLeg")));

    // Prepare Bones list
    BoneNameToMechPart.Add(FName("Calf_Left"), EMechPart::EMP_LeftLeg);
    BoneNameToMechPart.Add(FName("Foot0_Left"), EMechPart::EMP_LeftLeg);
    BoneNameToMechPart.Add(FName("Calf_Right"), EMechPart::EMP_RightLeg);
    BoneNameToMechPart.Add(FName("Foot0_Right"), EMechPart::EMP_RightLeg);

    BoneNameToMechPart.Add(FName("Upperarm_Left"), EMechPart::EMP_LeftArm);
    BoneNameToMechPart.Add(FName("Forearm_Left"), EMechPart::EMP_LeftArm);
    BoneNameToMechPart.Add(FName("Hand_Left"), EMechPart::EMP_LeftArm);

    BoneNameToMechPart.Add(FName("Upperarm_Right"), EMechPart::EMP_RightArm);
    BoneNameToMechPart.Add(FName("Forearm_Right"), EMechPart::EMP_RightArm);
    BoneNameToMechPart.Add(FName("Hand_Right"), EMechPart::EMP_RightArm);

    BoneNameToMechPart.Add(FName("Torso_Pitch"), EMechPart::EMP_Torso);
    BoneNameToMechPart.Add(FName("Pelvis_Center_Front"), EMechPart::EMP_Torso);
    BoneNameToMechPart.Add(FName("Clavicle_Left"), EMechPart::EMP_Torso);
    BoneNameToMechPart.Add(FName("Clavicle_Right"), EMechPart::EMP_Torso);
    BoneNameToMechPart.Add(FName("Thigh_Left"), EMechPart::EMP_Torso);
    BoneNameToMechPart.Add(FName("Thigh_Right"), EMechPart::EMP_Torso);

    BoneNameToMechPart.Add(FName("Cockpit"), EMechPart::EMP_Head);
}


// Called when the game starts
void UMRMechLivingComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache OwnerMech
    OwnerMech = Cast<AMRMech>(GetOwner());

    // Subscribe to Health Container Change State delegates
    for (auto HealthContainer : HealthContainerList)
    {
        UMRHealthComponent* HealthComponent = HealthContainer.Get<1>();
        if (HealthComponent)
        {
            HealthComponent->OnHealthStateChanged.AddDynamic(this, &UMRMechLivingComponent::OnHealthContainerStateChanged);
            HealthComponent->MaxHealth = 300.f;
            HealthComponent->CurrentHealth = 300.f;
        }
    }
}


// Called every frame
void UMRMechLivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UMRHealthComponent* UMRMechLivingComponent::FindHealthContainerByBone(const FName BoneName)
{
    if (!BoneNameToMechPart.Contains(BoneName))
        return nullptr;

    if (!HealthContainerList.Contains(BoneNameToMechPart[BoneName]))
        return nullptr;

    return HealthContainerList[BoneNameToMechPart[BoneName]];
}

UMRHealthComponent* UMRMechLivingComponent::FindHealthContainerByLocation(const FVector NearLocation)
{
    float PreviousBoneDistance = 0.f;

    EMechPart NearestMechPart = EMechPart::EMP_Invalid;

    for (auto Item : BoneNameToMechPart)
    {
        FName& SocketName = Item.Get<0>();
        FVector SocketLocation = OwnerMech->GetMesh()->GetSocketLocation(SocketName);

        // DrawDebugSphere(GetWorld(), SocketLocation, 96.f, 16, FColor::Red, false, 5.f, 0, 1.f);

        const float Distance = (SocketLocation - NearLocation).Size();

        //ULog::Number(Distance, "Nearest bone check: ", SocketName.ToString());

        if (!PreviousBoneDistance || Distance < PreviousBoneDistance)
        {
            NearestMechPart = Item.Get<1>();
            PreviousBoneDistance = Distance;

            // If less than minimum - hit a bone
            if (Distance < MinNearDamageDistance)
            {
                break;
            }
        }
    }

    if (PreviousBoneDistance < MaxNearDamageDistance && NearestMechPart != EMechPart::EMP_Invalid && HealthContainerList.Contains(NearestMechPart))
    {
        return HealthContainerList[NearestMechPart];
    }

    return nullptr;
}

EMechPart UMRMechLivingComponent::FindMechPartByHealthComponent(const UMRHealthComponent* HealthComponent)
{
    for (auto HealthContainer : HealthContainerList)
    {
        if (HealthContainer.Get<1>() == HealthComponent)
        {
            return HealthContainer.Get<0>();
        }
    }

    return EMechPart::EMP_Invalid;
}

float UMRMechLivingComponent::TakeDamage(const float TakenDamage, FDamageTakenData const& DamageTakenData)
{
    return TakenDamage;
}

float UMRMechLivingComponent::TakePointDamage(const float TakenDamage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    return HealthContainerTakeDamage(TakenDamage, PointDamageEvent.HitInfo, EventInstigator, DamageCauser);
}

float UMRMechLivingComponent::TakeRadialDamage(float TakenDamage, FRadialDamageEvent const& RadialDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float TotalDamage = 0;

    for (auto HitResult : RadialDamageEvent.ComponentHits)
    {
        TotalDamage += HealthContainerTakeDamage(TakenDamage, HitResult, EventInstigator, DamageCauser);
    }

    return TotalDamage;
}

float UMRMechLivingComponent::HealthContainerTakeDamage(float TakenDamage, FHitResult const& HitResult, AController* EventInstigator, AActor* DamageCauser)
{
    if (bDebug)
    {
        DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 96.f, 16, FColor::Blue, false, 5.f, 0, 1.f);
    }

    // Try to find a DamageContainer
    UMRHealthComponent* DamageContainer = nullptr;
    if (!HitResult.BoneName.IsNone())
    {
        DamageContainer = FindHealthContainerByBone(HitResult.BoneName);
    }

    if (!DamageContainer)
    {
        DamageContainer = FindHealthContainerByLocation(HitResult.ImpactPoint);
    }

    if (DamageContainer)
    {
        FDamageTakenData DamageTakenData = FDamageTakenData();
        DamageTakenData.DamageDealer = DamageCauser;
        return DamageContainer->TakeDamage(TakenDamage, DamageTakenData);
    }

    return 0.f;
}

EHealthState UMRMechLivingComponent::GetHealthState()
{
    return HealthState;
}

UMRHealthComponent* UMRMechLivingComponent::GetHealthContainer(const EMechPart InMechPart)
{
    if (HealthContainerList.Contains(InMechPart))
    {
        return HealthContainerList[InMechPart];
    }

    return nullptr;
}

void UMRMechLivingComponent::OnHealthContainerStateChanged(const FHealthStateChangedParams Params)
{
    
    if (Params.CurrentState == EHealthState::EHS_Damaged)
    {
        ULog::Success("Mech Part Damaged!!!", LO_Both);
        const EMechPart MechPart = FindMechPartByHealthComponent(Params.HealthComponent);
        if (MechPart != EMechPart::EMP_Invalid)
        {
            OwnerMech->DamagePart(MechPart);
        }
    } else if (Params.CurrentState == EHealthState::EHS_Destroyed)
    {
        ULog::Success("Mech Part Destroyed!!!", LO_Both);
        
        const EMechPart MechPart = FindMechPartByHealthComponent(Params.HealthComponent);
        if (MechPart != EMechPart::EMP_Invalid)
        {
            OwnerMech->DestroyPart(MechPart);
        }

        // Death
        if (MechPart == EMechPart::EMP_Torso || MechPart == EMechPart::EMP_Head)
        {
            HealthState = EHealthState::EHS_Destroyed;
            OwnerMech->Death();           
        }
    }
}
