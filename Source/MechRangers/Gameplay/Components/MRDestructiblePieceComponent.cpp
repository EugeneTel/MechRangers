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
	HealthContainer = CreateDefaultSubobject<UMRHealthComponent>(TEXT("HealthComponent"));
	InitialMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InitialMeshComponent"));
	InitialMeshComponent->SetupAttachment(this);
	InitialMeshComponent->SetCollisionProfileName(FName("BlockAll"));
	
	DamagedMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DamagedMeshComponent"));
	DamagedMeshComponent->SetupAttachment(this);
	DamagedMeshComponent->SetCollisionProfileName(FName("BlockAll"));
	DamagedMeshComponent->SetHiddenInGame(true);
}

// Called when the game starts
void UMRDestructiblePieceComponent::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe on Delegates
	HealthContainer->OnHealthStateChangedDelegate.BindUObject(this, &UMRDestructiblePieceComponent::OnHealthContainerStateChanged);
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
		InitialMeshComponent->SetStaticMesh(InitialMesh);
	}
}

void UMRDestructiblePieceComponent::PostEditComponentMove(bool bFinished)
{
	Super::PostEditComponentMove(bFinished);

	// @TODO: Check do we really need it?
	if (InitialMesh)
	{
		InitialMeshComponent->SetStaticMesh(InitialMesh);
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

void UMRDestructiblePieceComponent::OnHealthContainerStateChanged(UMRHealthComponent* InHealthContainer, EHealthState InHealthState)
{
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
	if (DamagedPieceData.MeshAsset)
	{
		InitialMeshComponent->SetStaticMesh(DamagedPieceData.MeshAsset);
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
	if (DestroyedPieceData.MeshAsset)
	{
		InitialMeshComponent->SetStaticMesh(DestroyedPieceData.MeshAsset);
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


