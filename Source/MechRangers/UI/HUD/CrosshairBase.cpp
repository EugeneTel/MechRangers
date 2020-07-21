// Fill out your copyright notice in the Description page of Project Settings.


#include "CrosshairBase.h"

// Sets default values
ACrosshairBase::ACrosshairBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACrosshairBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrosshairBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

