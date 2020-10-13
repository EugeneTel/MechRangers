// Copyright PlatoSpace.com All Rights Reserved.

#include "MRDestructiblePieceComponent.h"
#include "Log.h"
#include "MRHealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UMRDestructiblePieceComponent::UMRDestructiblePieceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Create components
	HealthContainer = CreateDefaultSubobject<UMRHealthComponent>(TEXT("DestructiblePieceHealthContainer"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	//MeshComponent->SetupAttachment(this);
	// @TODO: Fix collision profile name
	MeshComponent->SetCollisionProfileName(FName("MechPawn"));
}

// Called when the game starts
void UMRDestructiblePieceComponent::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe on Delegates
	HealthContainer->OnHealthStateChanged.BindUObject(this, &UMRDestructiblePieceComponent::OnHealthContainerStateChanged);
}

void UMRDestructiblePieceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMRDestructiblePieceComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (InitialMesh)
	{
		MeshComponent->SetStaticMesh(InitialMesh);
	}
}

float UMRDestructiblePieceComponent::TakeDamage(const float Damage, FDamageTakenData const& DamageTakenData)
{
	//ULog::Number(Damage, "DestructiblePieceComponent::TakeDamage ", GetNameSafe(this), LO_Both);
	
	return HealthContainer->TakeDamage(Damage, DamageTakenData);
}

EHealthState UMRDestructiblePieceComponent::GetHealthState()
{
	return HealthContainer->GetHealthState();
}

UStaticMeshComponent* UMRDestructiblePieceComponent::GetMeshComponent() const
{
	return MeshComponent;
}

void UMRDestructiblePieceComponent::OnHealthContainerStateChanged(UMRHealthComponent* InHealthContainer, EHealthState InHealthState)
{
	ULog::Success("CHANGE STATE!!!!!!!!!!!!!!");
	if (InHealthState == EHealthState::EHS_Damaged)
	{
		Damaged();
	} else if (InHealthState == EHealthState::EHS_Destroyed)
	{
		Destroyed();
	}
}

void UMRDestructiblePieceComponent::Damaged()
{
	ULog::Success("DAMAGED!!!!!!!!!!!!!!");
	if (DamagedPieceData.MeshAsset)
	{
		MeshComponent->SetStaticMesh(DamagedPieceData.MeshAsset);
	}

	if (DamagedPieceData.ParticleList.Num() > 0)
	{
		for (FDestructibleParticleData& DestructibleParticleData : DamagedPieceData.ParticleList)
		{
			SpawnParticle(DestructibleParticleData);
		}
	}
}

void UMRDestructiblePieceComponent::Destroyed()
{
	ULog::Success("DESTROYED!!!!!!!!!!!!!!");
	if (DestroyedPieceData.MeshAsset)
	{
		MeshComponent->SetStaticMesh(DestroyedPieceData.MeshAsset);
	}

	if (DestroyedPieceData.ParticleList.Num() > 0)
	{
		for (FDestructibleParticleData& DestructibleParticleData : DestroyedPieceData.ParticleList)
		{
			SpawnParticle(DestructibleParticleData);
		}
	}

}

void UMRDestructiblePieceComponent::SpawnParticle(FDestructibleParticleData& DestructibleParticleData)
{
	const FVector SpawnLocation = GetComponentLocation() + DestructibleParticleData.Translation;
	
	if (DestructibleParticleData.ParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DestructibleParticleData.ParticleSystem,
			SpawnLocation,
			DestructibleParticleData.Rotation
		);
	}

	if (DestructibleParticleData.Audio)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestructibleParticleData.Audio, SpawnLocation);
	}
}


