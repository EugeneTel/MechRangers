// Copyright PlatoSpace.com. All Rights Reserved.

#include "MRMech.h"
#include "MechComponents/MRMechMovementComponent.h"
#include "MechComponents/MRMechLivingComponent.h"

// Sets default values
AMRMech::AMRMech(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UMRMechMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Defaults
	LivingComponent = CreateDefaultSubobject<UMRMechLivingComponent>(TEXT("LivingComponent"));
}

// Called when the game starts or when spawned
void AMRMech::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMRMech::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMRMech::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


