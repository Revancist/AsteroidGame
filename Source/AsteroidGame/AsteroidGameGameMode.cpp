// Copyright Epic Games, Inc. All Rights Reserved.

#include "AsteroidGameGameMode.h"
#include "AsteroidGamePawn.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h" 


void AAsteroidGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	//AAsteroidGamePawn* MyCharacter = Cast<AAsteroidGamePawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerHUDClass != nullptr)
	{
		// Create selected widget
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (CurrentWidget != nullptr)
		{
			// Add widget to viewport
			CurrentWidget->AddToViewport();
		}
	}
}

AAsteroidGameGameMode::AAsteroidGameGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AAsteroidGamePawn::StaticClass();
}

