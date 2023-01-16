// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class ASTEROIDGAME_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	// Asteroid counter
	int AsteroidCounter;

	// Max allowed asteroids
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		int AsteroidMax;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Asteroid to spawn
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class AAsteroidGameAsteroid> WhatToSpawn;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Returns the WhereToSpawn subobject
	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

	// Find a random point within the BoxComponent
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	// Handles spawning of asteroids
	void SpawnAsteroid();

private:
	// BoxComponent to specify where asteroids should spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhereToSpawn;

};
