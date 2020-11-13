// Copyright PlatoSpace.com All Rights Reserved.

#include "MissionSystem/MRMissionTargetComponent.h"
#include "MechRangers/Modes/MRGameMode.h"

// Sets default values for this component's properties
UMRMissionTargetComponent::UMRMissionTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMRMissionTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* TargetActor = GetOwner();
	if (TargetActor)
	{
		if (!TargetActor->GetClass()->ImplementsInterface(UMRDamageTakerInterface::StaticClass()))
			return;

		IMRDamageTakerInterface* NewTarget = Cast<IMRDamageTakerInterface>(TargetActor);
		if (!NewTarget)
			return;

		NewTarget->OnDeath().AddUObject(this, &UMRMissionTargetComponent::OnActorDeath);
	}
}

void UMRMissionTargetComponent::OnRegister()
{
	Super::OnRegister();

	// if (UWorld* World = GetWorld())
	// {
	// 	if (AMRGameMode* GameMode = World->GetAuthGameMode<AMRGameMode>())
	// 	{
	// 		GameMode->SetPlayerSlomoSphere(this);
	// 	}
	// }
}

void UMRMissionTargetComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UMRMissionTargetComponent::OnActorDeath(AActor* DeadActor)
{
	InformTargetProgress(EMissionTargetAction::Destroy);
}

void UMRMissionTargetComponent::InformTargetProgress(EMissionTargetAction TargetAction)
{
	if (UWorld* World = GetWorld())
	{
		AMRGameMode* GameMode = Cast<AMRGameMode>(World->GetAuthGameMode());
		if (GameMode)
		{
			if (UMRMissionManagerComponent* MissionManager = GameMode->GetMissionManager())
			{
				MissionManager->UpdateTargetProgress(TargetInfo, TargetAction);
			}
		}
	}
}
