// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AsteroidGameGameMode.generated.h"

UCLASS(MinimalAPI)
class AAsteroidGameGameMode : public AGameModeBase
{
	GENERATED_BODY()
		
	// Override BeginPlay() from the base class
	virtual void BeginPlay() override;
	
public:
	AAsteroidGameGameMode();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD")
		TSubclassOf<class UUserWidget> PlayerHUDClass;

	UPROPERTY()
		class UUserWidget* CurrentWidget;
	
};



