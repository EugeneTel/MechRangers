// Copyright PlatoSpace.com All Rights Reserved.


#include "MRGameMode.h"
#include "MechRangers/Gameplay/Managers/MRUnitManagerComponent.h"
#include "Mech/Structure/MRMechLoadoutDataAsset.h"
#include "Log.h"
#include "Pilot/MRPilotInterface.h"

AMRGameMode::AMRGameMode()
{
    UnitManager = CreateDefaultSubobject<UMRUnitManagerComponent>(TEXT("UnitManager"));
    MissionManager = CreateDefaultSubobject<UMRMissionManagerComponent>(TEXT("MissionManager"));
}

APawn* AMRGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
    // Spawn Pilot
    APawn* NewPawn = Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, SpawnTransform);

    // Spawn Mech only if we have relevant data for it
    if (!MechClass || !MechLoadoutAsset)
    {
        return NewPawn;
    }
    
    if (!NewPawn || !NewPawn->GetClass()->ImplementsInterface(UMRPilotInterface::StaticClass()))
    {
        UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: DefaultPawn MUST implement the PilotInterface! Set Correct Pilot."), *GetNameSafe(MechClass));
        return NewPawn;
    }

    // Spawn Mech
    AMRMech* NewMech = UnitManager->SpawnMech(MechClass, MechLoadoutAsset->GetLoadout(), SpawnTransform);
    if (NewMech)
    {
        Cast<IMRPilotInterface>(NewPawn)->Execute_SitIntoMech(NewPawn, NewMech);
    }

    // Possess Mech By Pilot
    NewPlayer->Possess(NewMech);

    return NewPawn;
}

UMRUnitManagerComponent* AMRGameMode::GetUnitManager() const
{
    return UnitManager;
}

UMRMissionManagerComponent* AMRGameMode::GetMissionManager() const
{
    return MissionManager;
}
