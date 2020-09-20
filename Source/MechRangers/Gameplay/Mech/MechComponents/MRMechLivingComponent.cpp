// Copyright PlatoSpace.com All Rights Reserved.

#include "MRMechLivingComponent.h"
#include "MRMechHealthComponent.h"
#include "Log.h"
#include "UObject/ObjectMacros.h"


// Sets default values for this component's properties
UMRMechLivingComponent::UMRMechLivingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Create health containers
	HealthContainers.Add(EMechPart::EMP_Head, CreateDefaultSubobject<UMRMechHealthComponent>(TEXT("HealthContainer_Head")));
	HealthContainers.Add(EMechPart::EMP_Torso, CreateDefaultSubobject<UMRMechHealthComponent>(TEXT("HealthContainer_Torso")));
	HealthContainers.Add(EMechPart::EMP_RightArm, CreateDefaultSubobject<UMRMechHealthComponent>(TEXT("HealthContainer_RightArm")));
	HealthContainers.Add(EMechPart::EMP_LeftArm, CreateDefaultSubobject<UMRMechHealthComponent>(TEXT("HealthContainer_LeftArm")));
	HealthContainers.Add(EMechPart::EMP_RightLeg, CreateDefaultSubobject<UMRMechHealthComponent>(TEXT("HealthContainer_RightLeg")));
	HealthContainers.Add(EMechPart::EMP_LeftLeg, CreateDefaultSubobject<UMRMechHealthComponent>(TEXT("HealthContainer_LeftLeg")));

	BoneNameToMechPart.Add(FName("Calf_Left"), EMechPart::EMP_LeftLeg);
	BoneNameToMechPart.Add(FName("Calf_Right"), EMechPart::EMP_RightLeg);
}


// Called when the game starts
void UMRMechLivingComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMRMechLivingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UMRMechHealthComponent* UMRMechLivingComponent::FindDamageHealthContainer(const FName BoneName)
{
	if (!BoneNameToMechPart.Contains(BoneName))
		return nullptr;

	if (!HealthContainers.Contains(BoneNameToMechPart[BoneName]))
		return nullptr;

	return 	HealthContainers[BoneNameToMechPart[BoneName]];
}

float UMRMechLivingComponent::TakeDamage(const float TakenDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ULog::Success("Mech Takes Damage!!!");
	return TakenDamage;
}

float UMRMechLivingComponent::TakePointDamage(const float TakenDamage, FPointDamageEvent const& PointDamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const FName DamageBoneName = PointDamageEvent.HitInfo.BoneName;
	if (DamageBoneName.IsNone())
		return 0.f;

	UMRMechHealthComponent* DamageContainer = FindDamageHealthContainer(DamageBoneName);
	if (DamageContainer)
	{
		DamageContainer->TakeDamage(TakenDamage);
		return TakenDamage;
	}

	return 0.f;
}



