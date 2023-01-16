// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AsteroidGameAsteroid.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the Box Component to represent the spawn volume
	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	RootComponent = WhereToSpawn;

	//Set Asteroid Counter to 0 and max asteroids to default
	AsteroidCounter = 0;
	AsteroidMax = 4;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < AsteroidMax; i++)
	{
		SpawnAsteroid();
	}
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Returns a random point in volume
FVector ASpawnVolume::GetRandomPointInVolume()
{
	FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
	FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

// Spawn asteroid in world
void ASpawnVolume::SpawnAsteroid()
{
	// Check if something to spawn is set
	if (WhatToSpawn != NULL)
	{
		// Check for valid World
		UWorld* const World = GetWorld();
		if (World)
		{
			// Set spawn parameters
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Get a random location to spawn at
			FVector SpawnLocation = GetRandomPointInVolume();
			
			// Get a random rotation for the spawned asteroid
			FRotator SpawnRotation;
			SpawnRotation *= 0;
			/*
			SpawnRotation.Yaw = FMath::FRand() * 360.0f;
			SpawnRotation.Pitch = FMath::FRand() * 360.0f;
			SpawnRotation.Roll = FMath::FRand() * 360.0f;
			*/
			
			// Spawn the asteroid
			AAsteroidGameAsteroid* const SpawnedAsteroid = World->SpawnActor<AAsteroidGameAsteroid>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			AsteroidCounter++;
		}
	}
}

