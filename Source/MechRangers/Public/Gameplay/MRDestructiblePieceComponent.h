// Copyright PlatoSpace.com All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/SceneComponent.h"
#include "MechRangersTypes.h"
#include "LivingSystem/MRDamageTakerInterface.h"
#include "Sound/SoundCue.h"
#include "UObject/ObjectMacros.h"

#include "MRDestructiblePieceComponent.generated.h"

class UMRHealthComponent;

/** Element placement position */
UENUM(BlueprintType)
enum class EPlacementPosition : uint8
{
	EPP_Center UMETA(DisplayName = "Center"),
    EPP_Top UMETA(DisplayName = "Top"),
    EPP_Bottom UMETA(DisplayName = "Bottom"),
    EPP_Front UMETA(DisplayName = "Front"),
    EPP_Back UMETA(DisplayName = "Back"),
    EPP_Left UMETA(DisplayName = "Left"),
    EPP_Right UMETA(DisplayName = "Right"),
    EPP_Origin UMETA(DisplayName = "Origin")
};

/** Particle for destruction */
USTRUCT(BlueprintType)
struct FDestructibleParticleData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UParticleSystem* ParticleSystem = nullptr;

	/** Where particle will be placed according to the Translation field. TODO: Implement functionality */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EPlacementPosition RelativePlacement;

	/** Relative translation where particle system will be spawned */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector Translation;

	/** Particle rotation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FRotator Rotation;

	/** Particle audio */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USoundCue* Audio = nullptr;

	FDestructibleParticleData()
        : RelativePlacement(EPlacementPosition::EPP_Center),
        Translation(FVector::ZeroVector),
        Rotation(FRotator::ZeroRotator)
	{}
};

USTRUCT(BlueprintType)
struct FDestructiblePieceData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMesh* MeshAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FDestructibleParticleData> ParticleList;

	FDestructiblePieceData()
	{
		MeshAsset = nullptr;
		ParticleList = TArray<FDestructibleParticleData>();
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MECHRANGERS_API UMRDestructiblePieceComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadWrite)
	UMRHealthComponent* HealthContainer;

	UPROPERTY(BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Sets default values for this component's properties
	UMRDestructiblePieceComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable)
	float TakeDamage(float Damage, FDamageTakenData const& DamageTakenData);

	/** Get health state of the current component */
	UFUNCTION(BlueprintCallable)
	EHealthState GetHealthState();

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetMeshComponent() const;
	
protected:

	UPROPERTY(Category=Destructible, EditDefaultsOnly, BlueprintReadWrite)
	UStaticMesh* InitialMesh;

	UPROPERTY(Category=Destructible, EditAnywhere, BlueprintReadWrite)
	FDestructiblePieceData DamagedPieceData;

	UPROPERTY(Category=Destructible, EditAnywhere, BlueprintReadWrite)
	FDestructiblePieceData DestroyedPieceData;
	
	/** When set to true, damage will not affect */
	UPROPERTY(Category=Destructible, EditAnywhere, BlueprintReadWrite)
	bool bIsImmortal;

	/** When set to true, blow off on contact with any Mech */
	UPROPERTY(Category=Destructible, EditAnywhere, BlueprintReadWrite)
	bool bMechContact;

	/** Subscribed to the Health Component Health State Changed Delegate */
	UFUNCTION()
	void OnHealthContainerStateChanged(FHealthStateChangedParams Params);
	
	/** Spawn particle for destructible piece */
	void SpawnParticle(FDestructibleParticleData& DestructibleParticleData);

public:

	/** Set destructible component state to Damaged */
	UFUNCTION(BlueprintCallable)
	void Damaged();
	
	/** Set destructible component state to Destroyed */
	UFUNCTION(BlueprintCallable)
    void Destroyed();

};
