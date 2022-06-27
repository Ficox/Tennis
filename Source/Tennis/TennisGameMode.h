// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TennisGameMode.generated.h"


UCLASS(minimalapi)
class ATennisGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATennisGameMode();
	virtual void BeginPlay() override;
	// Scoring system
	const TArray<FString> points = {"0", "15", "30", "40", "AD"};
	int SetsTeam1 = 0;
	int SetsTeam2 = 0;
	int GemsTeam1 = 0;
	int GemsTeam2 = 0;
	int ptsTeam1 = 0;
	int ptsTeam2 = 0;
	int servingTeam = 0;
	void AddPoint(int teamNumber);
	void ShowScore();

	/** Widget class to spawn display */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTennisPointsUserWidget> TennisUIClass;

	/** Widget instance */
	UPROPERTY()
	class UTennisPointsUserWidget* TennisUI;

	UFUNCTION()
	UTennisPointsUserWidget* GetTennisUI();

	void TurnOnInput();

	// Reset game for new point
	void ResetAll();
};



