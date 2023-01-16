// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidGameAsteroid.h"
#include "AsteroidGamePawn.h"
#include "SpawnVolume.h"
#include "Components/CapsuleComponent.h"
#include "AsteroidGameProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAsteroidGameAsteroid::AAsteroidGameAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*
	// Create the static mesh component
	AsteroidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AsteroidMesh"));
	AsteroidMesh->SetSimulatePhysics(false);
	*/

	//RootComponent = AsteroidMesh;
	/*
	AsteroidMesh->SetSimulatePhysics(false);
	AsteroidMesh->SetEnableGravity(false);
	AsteroidMesh->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	RootComponent = AsteroidMesh;
	*/
	
	// Set asteroid's velocity
	velocity = 1000.0f * (2.0f * FVector(FMath::FRand(), FMath::FRand(), 0.0f) - 1.0f);
	velocity.Z = 0.0f;
	/*
	MoveSpeed = FMath::RandRange(100.f, 1000.f);
	ForwardValue = FMath::RandRange(-1.f, 1.f);
	RightValue = FMath::RandRange(-1.f, 1.f);
	*/

		// Set up the Capsule Collision
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	//CapsuleCollision->SetSimulatePhysics(true);
	//CapsuleCollision->SetEnableGravity(false);
	//CapsuleCollision->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	
	RootComponent = CapsuleCollision;

		//CapsuleCollision->OnComponentHit.AddDynamic(this, &AAsteroidGameAsteroid::OnHit);
	//FAttachmentTransformRules attachRules(EAttachmentRule::KeepRelative, false);
	//CapsuleCollision->AttachToComponent(RootComponent, attachRules);
	
	// Set default points
	points = 100;
}

// Called when the game starts or when spawned
void AAsteroidGameAsteroid::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAsteroidGameAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector location = GetActorLocation();
	location += velocity * DeltaTime;

	SetActorLocation(location, true);

	/*
								//Calculate movement and move asteroid

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement and rotation
	Movement = MoveDirection * MoveSpeed * DeltaTime;
	const FRotator NewRotation = Movement.Rotation();

	// Structure containing information about one hit of a trace, such as point of impact and surface normal at that point.
	FHitResult Hit(1.f);


	RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
	// Check if asteroid hit something
	if (Hit.IsValidBlockingHit())
	{
		// Calculate deflection
		const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
		Movement = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);

		// Move asteroid in new direction
		RootComponent->MoveComponent(Movement, NewRotation, true);
	}
	
	*/
					/* Code for teleporting asteroid if outside screen */

	//Get Player's screen location
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Project asteroid's world location to screen location
	FVector2D ActorScreenLocation;
	FVector ActorWorldLocation = GetActorLocation();

	PlayerController->ProjectWorldLocationToScreen(ActorWorldLocation, ActorScreenLocation, true);

	// Check if asteroid is outside of viewport and teleport
	FVector newActorLocation;
	bool needsToTeleport = false;

	if (ActorScreenLocation.X > ViewportSizeX - 204)
	{

		newActorLocation.X = ActorWorldLocation.X;
		newActorLocation.Y = -1 * ActorWorldLocation.Y - 100;
		newActorLocation.Z = ActorWorldLocation.Z;

		needsToTeleport = true;

	}
	else if (ActorScreenLocation.X < 0)
	{

		newActorLocation.X = ActorWorldLocation.X;
		newActorLocation.Y = -1 * ActorWorldLocation.Y - 200;
		newActorLocation.Z = ActorWorldLocation.Z;

		needsToTeleport = true;
	}

	if (ActorScreenLocation.Y > ViewportSizeY)
	{

		newActorLocation.X = -1 * ActorWorldLocation.X - 100;
		newActorLocation.Y = ActorWorldLocation.Y;
		newActorLocation.Z = ActorWorldLocation.Z;

		needsToTeleport = true;

	}
	else if (ActorScreenLocation.Y < 0)
	{

		newActorLocation.X = -1 * ActorWorldLocation.X + 100;
		newActorLocation.Y = ActorWorldLocation.Y;
		newActorLocation.Z = ActorWorldLocation.Z;

		needsToTeleport = true;
	}

	// Teleport asteroid to opposite edge of screen
	if (needsToTeleport)
	{
		SetActorLocation(newActorLocation, false);
	}
}
// Called when actor is hit by something
void AAsteroidGameAsteroid::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect asteroid on hit
	FVector Direction = FMath::GetReflectionVector(HitNormal, velocity.GetSafeNormal());
	velocity = velocity.Size() * Direction;
	velocity.Z = 0.0f;
	
	// Check if asteroid is hit by player's bullet
	if (Cast<AAsteroidGameProjectile>(Other) != nullptr)
	{
		// Play particle effect at hit asteroid's location
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, GetActorLocation(), GetActorRotation());
		
		// Get spawn volume and spawn another asteroid
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnVolume::StaticClass());
		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundActor);

		if (SpawnVolume != nullptr)
		{
			SpawnVolume->SpawnAsteroid();
		}

		// Get player character and add points
		FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AAsteroidGamePawn::StaticClass());
		AAsteroidGamePawn* PlayerActor = Cast<AAsteroidGamePawn>(FoundActor);

		if (PlayerActor != nullptr)
		{
			PlayerActor->AddPoints(points);
		}

		// Destroy hit asteroid
		Destroy();
	}
	// Check if asteroid is hit by player's pawn
	else if (Cast<AAsteroidGamePawn>(Other) != nullptr)
	{
		// Decrease player's life
		Cast<AAsteroidGamePawn>(Other)->DecreaseLife();
	}
}

/*
// Called when actor is hit by something
void AAsteroidGameAsteroid::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherComp != NULL))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *OtherActor->GetName()));
	}
	if (OtherActor->IsA(AAsteroidGameAsteroid::StaticClass()))
	{
		&AAsteroidGamePawn::GameOver;
	}
	else 
	{

	}
}
*/
