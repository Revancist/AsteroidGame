// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AsteroidGameAsteroid.generated.h"

class UCapsuleComponent;

UCLASS()
class ASTEROIDGAME_API AAsteroidGameAsteroid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroidGameAsteroid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
		class UParticleSystem* ParticleEffect;

	// Amount of points added to score after shooting asteroid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score Points")
		int points;

private:

	// Static mesh to represent the asteroid
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Asteroid", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AsteroidMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Capsule", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleCollision;

	// Asteroid's velocity
	FVector velocity;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/*
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	*/

	/*
public:
	// The speed the asteroid moves around the level
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;

	// Asteroid direction
	float ForwardValue;
	float RightValue;
	FVector MoveDirection;
	FVector Movement;
	*/
};
