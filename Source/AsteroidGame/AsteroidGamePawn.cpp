// Copyright Epic Games, Inc. All Rights Reserved.
#pragma optimize("", off)

#include "AsteroidGamePawn.h"
#include "AsteroidGameProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Blueprint/UserWidget.h" 

const FName AAsteroidGamePawn::MoveForwardBinding("MoveForward");
const FName AAsteroidGamePawn::MoveRightBinding("MoveRight");
const FName AAsteroidGamePawn::FireForwardBinding("FireForward");
const FName AAsteroidGamePawn::FireRightBinding("FireRight");

const FName AAsteroidGamePawn::FireBinding("FireGun");

AAsteroidGamePawn::AAsteroidGamePawn()
{	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	/*
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-80.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	
	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm
	*/

	// Movement
	MoveSpeed = 1000.0f;

	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.5f;  //Default 0.1f
	bCanFire = true;

	// Life
	Life = 3;
	vulnerable = true;

	// Score
	score = 0;
}

void AAsteroidGamePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction(FireBinding, IE_Pressed, this, &AAsteroidGamePawn::FireShotForward);
}

void AAsteroidGamePawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
		
		if (Hit.IsValidBlockingHit())
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
	}
	
	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);

	// Try and fire a shot
	//FireShotDirection(FireDirection);

	// Get Player's Screen location
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Project player's world location to screen location
	FVector2D ActorScreenLocation;

	FVector ActorWorldLocation = GetActorLocation();
	//FVector ActorWorldDirection;

	PlayerController->ProjectWorldLocationToScreen(ActorWorldLocation, ActorScreenLocation, true);
	bool NeedsToTeleport = false;

	// Check if player is outside of viewport and teleport

	FVector newActorLocation;
	
	if (ActorScreenLocation.X > ViewportSizeX - 204)
	{

		newActorLocation.X = ActorWorldLocation.X;
		newActorLocation.Y = -1 * ActorWorldLocation.Y - 100;
		newActorLocation.Z = ActorWorldLocation.Z;

		NeedsToTeleport = true;

	}else if (ActorScreenLocation.X < 0)
	{

		newActorLocation.X = ActorWorldLocation.X;
		newActorLocation.Y = -1 * ActorWorldLocation.Y - 200;
		newActorLocation.Z = ActorWorldLocation.Z;

		NeedsToTeleport = true;
	}

	if (ActorScreenLocation.Y > ViewportSizeY)
	{

		newActorLocation.X = -1 * ActorWorldLocation.X - 100;
		newActorLocation.Y = ActorWorldLocation.Y;
		newActorLocation.Z = ActorWorldLocation.Z;

		NeedsToTeleport = true;

	}else if (ActorScreenLocation.Y < 0)
	{

		newActorLocation.X = -1 * ActorWorldLocation.X + 100;
		newActorLocation.Y = ActorWorldLocation.Y;
		newActorLocation.Z = ActorWorldLocation.Z;

		NeedsToTeleport = true;
	}

	/*
	if (ActorScreenLocation.X > ViewportSizeX)
	{
		ActorScreenLocation.X = 0;
		NeedsToTeleport = true;
	}
	if (ActorScreenLocation.Y > ViewportSizeY)
	{
		ActorScreenLocation.Y = 0;
		NeedsToTeleport = true;
	}

	if (ActorScreenLocation.X < 0)
	{
		ActorScreenLocation.X = ViewportSizeX;
		NeedsToTeleport = true;
	}
	if (ActorScreenLocation.Y < 0)
	{
		ActorScreenLocation.Y = ViewportSizeY;
		NeedsToTeleport = true;
	}
	*/
	// Teleport player to opposite edge of screen
	if(NeedsToTeleport)
	{
		//FVector newActorLocation;
		//PlayerController->DeprojectScreenPositionToWorld(ActorScreenLocation.X, ActorScreenLocation.Y, newActorLocation, ActorWorldDirection);
		//newActorLocation.X = -1 * ActorWorldLocation.X;
		//newActorLocation.Z = ActorWorldLocation.Z;

		SetActorLocation(newActorLocation, false);
	}
}

void AAsteroidGamePawn::FireShotDirection(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				// spawn the projectile
				World->SpawnActor<AAsteroidGameProjectile>(SpawnLocation, FireRotation);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AAsteroidGamePawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void AAsteroidGamePawn::ShotTimerExpired()
{
	bCanFire = true;
}

void AAsteroidGamePawn::FireShotForward()
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		const FRotator MoveRotation = GetActorForwardVector().Rotation();
		// Spawn projectile at an offset from this pawn
		const FVector SpawnLocation = GetActorLocation() + MoveRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			// spawn the projectile
			World->SpawnActor<AAsteroidGameProjectile>(SpawnLocation, MoveRotation);
		}

		bCanFire = false;
		World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AAsteroidGamePawn::ShotTimerExpired, FireRate);

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		bCanFire = false;
	}
}

// Decrease player's life point
void AAsteroidGamePawn::DecreaseLife()
{
	// Check if player is vulnerable
	if (vulnerable)
	{
		Life--;

		// Check for game over (player has 0 life)
		if (Life == 0)
		{
			GameOver();
			return;
		}

		UWorld* const World = GetWorld();
		if(World != nullptr)
		{
			// Set timer for invincibility
			World->GetTimerManager().SetTimer(TimerHandle_InvincibilityExpired, this, &AAsteroidGamePawn::DamageTimerExpired, 2.0f);

			// Set vulnerable flag
			vulnerable = false;
		}

		// Subtract score points
		SubtractPoints(200);
	}
}

// Called when player has lost the game
void AAsteroidGamePawn::GameOver()
{
	// Disable player input
	AAsteroidGamePawn::DisableInput(GetWorld()->GetFirstPlayerController());

	// Hide player's pawn
	this->SetActorHiddenInGame(true);

	// Play particle effect at player's location
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, GetActorLocation(), GetActorRotation());

}

int AAsteroidGamePawn::GetScore()
{
	return score;
}

void AAsteroidGamePawn::AddPoints(int Points)
{
	score += Points;
}

void AAsteroidGamePawn::SubtractPoints(int Points)
{
	score -= Points;
	
	// Cap score at 0
	if (score < 0)
	{
		score = 0;
	}
}

void AAsteroidGamePawn::DamageTimerExpired()
{
	vulnerable = true;
}

