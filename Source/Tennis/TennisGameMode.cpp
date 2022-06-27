// Copyright Epic Games, Inc. All Rights Reserved.

#include "TennisGameMode.h"
#include "TennisCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TennisPointsUserWidget.h"
#include <GameFramework/PlayerStart.h>
#include "Kismet/GameplayStatics.h"
#include "string"
#include <Blueprint/UserWidget.h>
#include <Tennis/TennisBall.h>
#include "TennisPointsUserWidget.h"

ATennisGameMode::ATennisGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	TennisUI = nullptr;
	TennisUIClass = nullptr;
}
void ATennisGameMode::BeginPlay()
{
	Super::BeginPlay();
	TSubclassOf<APlayerStart> classToFind;
	classToFind = APlayerStart::StaticClass();
	TArray<AActor*> foundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundPlayers);
	FString IntAsString = FString::FromInt(foundPlayers.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, IntAsString);
	APlayerStart* PS = (APlayerStart*)foundPlayers.Pop();
	FActorSpawnParameters PlayerSpawnParameters{};
	PlayerSpawnParameters.Owner = this;
	ATennisCharacter* newTennisPlayer= (ATennisCharacter*)UGameplayStatics::CreatePlayer(GetWorld(), -1, true);
	// rotate player





}

void ATennisGameMode::AddPoint(int teamNumber)
{
	if (teamNumber == 1)
	{
		// player2 has 4 pts (ADV)
		if (ptsTeam1 == 3 && ptsTeam2 == 4) 
		{
			ptsTeam2--;
		}
		// deuce
		else if (ptsTeam1 == 3 && ptsTeam2 ==3) 
		{
			ptsTeam1++;
		}
		// gem point
		else if (ptsTeam1 >= 3)
		{
			ptsTeam1 = 0;
			ptsTeam2 = 0;
			if (GemsTeam1 == 5) 
			{
				GemsTeam1 = 0;
				GemsTeam2 = 0;
				SetsTeam1++;
			}
			else
			{
				GemsTeam1++;
			}
		}
		// normal point
		else 
		{
			ptsTeam1++;
			// show msg if there are set points
			if (GemsTeam1 == 5 && ptsTeam1 == 3 && ptsTeam1 - ptsTeam2 > 0)
			{
				if (ptsTeam1 - ptsTeam2 == 1)
				{
					TennisUI->ShowMSG("SET POINT");
				}
				else if (ptsTeam1 - ptsTeam2 == 2)
				{
					TennisUI->ShowMSG("TWO SET POINTS");
				}
				else if (ptsTeam1 - ptsTeam2 == 3)
				{
					TennisUI->ShowMSG("THREE SET POINTS");
				}
			}
			// show msg if there are break points
			if (servingTeam < 0 && ptsTeam1 == 3 && ptsTeam2 < 3 && !(GemsTeam1 == 5))
			{
				if (ptsTeam1 - ptsTeam2 == 1)
				{
					TennisUI->ShowMSG("BREAK POINT");
				}
				else if (ptsTeam1 - ptsTeam2 == 2)
				{
					TennisUI->ShowMSG("TWO BREAK POINTS");
				}
				else if (ptsTeam1 - ptsTeam2 == 3)
				{
					TennisUI->ShowMSG("THREE BREAK POINTS");
				}
				
			}
		}
	}
	else
	{
		// player1 has 4 pts (ADV)
		if (ptsTeam2 == 3 && ptsTeam1 == 4)
		{
			ptsTeam1--;
		}
		// deuce
		else if (ptsTeam2 == 3 && ptsTeam1 == 3)
		{
			ptsTeam2++;
		}
		// gem point
		else if (ptsTeam2 >= 3)
		{
			ptsTeam1 = 0;
			ptsTeam2 = 0;
			if (GemsTeam2 == 5) {
				GemsTeam2 = 0;
				GemsTeam1 = 0;
				SetsTeam2++;
			}
			else 
			{
				GemsTeam2++;
			}
			
		}
		// normal point
		else
		{
			ptsTeam2++;
			// show msg if there are set points
			if (GemsTeam2 == 5 && ptsTeam2==3 && ptsTeam2 - ptsTeam1 > 0) 
			{
				if (ptsTeam2 - ptsTeam1 == 1)
				{
					TennisUI->ShowMSG("SET POINT");
				}
				else if (ptsTeam2 - ptsTeam1 == 2)
				{
					TennisUI->ShowMSG("TWO SET POINTS");
				}
				else if (ptsTeam2 - ptsTeam1 == 3)
				{
					TennisUI->ShowMSG("THREE SET POINTS");
				}
			}
			// show msg if there are break points
			if (servingTeam > 0 && ptsTeam2 == 3 && ptsTeam1 < 3 && !(GemsTeam2 == 5)) 
			{
				if (ptsTeam2 - ptsTeam1 == 1) 
				{
					TennisUI->ShowMSG("BREAK POINT");
				}
				else if (ptsTeam2 - ptsTeam1 == 2)
				{
					TennisUI->ShowMSG("TWO BREAK POINTS");
				}
				else if (ptsTeam2 - ptsTeam1 == 3)
				{
					TennisUI->ShowMSG("THREE BREAK POINTS");
				}
				
			}
		}
	}
}

void ATennisGameMode::ShowScore()
{
	TennisUI->SetValues(SetsTeam1, SetsTeam2, GemsTeam1, GemsTeam2, points[ptsTeam1], points[ptsTeam2], servingTeam);
	FString outString1 = FString();
	FString outString2 = FString();
	
	/*
	outString1 = FString("Team1 ") + FString::FromInt(SetsTeam1) + FString(" ") + FString::FromInt(GemsTeam1) + FString(" ") + points[ptsTeam1];
	outString2 = FString("Team2 ") + FString::FromInt(SetsTeam2) + FString(" ") + FString::FromInt(GemsTeam2) + FString(" ") + points[ptsTeam2];
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, outString1);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, outString2);
	*/
}

UTennisPointsUserWidget* ATennisGameMode::GetTennisUI()
{
	return TennisUI;
}

void ATennisGameMode::TurnOnInput()
{
	TSubclassOf<ATennisCharacter> classToFind;
	classToFind = ATennisCharacter::StaticClass();
	TArray<AActor*> foundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundPlayers);
	for (size_t i = 0; i < foundPlayers.Num(); i++)
	{ 
		ATennisCharacter* TC = ((ATennisCharacter*)foundPlayers[i]);
		APlayerController* ATC = (APlayerController*)TC->GetController();
		TC->EnableInput(ATC);
	}
}

void ATennisGameMode::ResetAll()
{
	// Find all players and reset 
	TSubclassOf<ATennisCharacter> classToFind;
	classToFind = ATennisCharacter::StaticClass();
	TArray<AActor*> foundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundPlayers);
	// Find ball and reset
	TSubclassOf<ATennisBall> classToFind2;
	classToFind2 = ATennisBall::StaticClass();
	TArray<AActor*> foundBalls;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind2, foundBalls);
	ATennisBall* Ball = (ATennisBall*)foundBalls.Pop();
	for (size_t i = 0; i < foundPlayers.Num(); i++)
	{
		// Disable input until characters are ready for new point
		ATennisCharacter* TC = ((ATennisCharacter*)foundPlayers[i]);
		APlayerController * ATC = (APlayerController*)TC->GetController();
		TC->DisableInput(ATC);
		FTimerHandle TimerHandle;
		TC->IsShooting = false;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,this, &ATennisGameMode::TurnOnInput, 0.5, false);

		TC->ballLocation = (FVector(0.f, 0.f, 0.f));
		if (TC->playerID > -1)
		{
			if ((GemsTeam1 + GemsTeam2) % 2 == 1) {
				Ball->servingTeam = -1;
				servingTeam = -1;
				TC->isServing = false;
			}
			else 
			{
				TC->isServing = true;
			}
			if ((ptsTeam1 + ptsTeam2) % 2 == 1)
			{
				foundPlayers[i]->SetActorLocation(FVector(-1280.f, -180.f, 232.f));
			}
			else
			{
				foundPlayers[i]->SetActorLocation(FVector(-1280.f, 180.f, 232.f));
			}
		}
		else
		{
			if ((GemsTeam1 + GemsTeam2) % 2 == 0) {
				Ball->servingTeam = 1;
				servingTeam = 1;
				TC->isServing = false;
			}
			else
			{
				TC->isServing = true;
			}
			if ((ptsTeam1 + ptsTeam2) % 2 == 1)
			{
				foundPlayers[i]->SetActorLocation(FVector(1280.f, 180.f, 232.f));
			}
			else
			{
				foundPlayers[i]->SetActorLocation(FVector(1280.f, -180.f, 232.f));
			}
			
		}

	}

	if ((GemsTeam1 + GemsTeam2) % 2 == 0) {
		if ((ptsTeam1 + ptsTeam2) % 2 == 1)
		{
			Ball->SetActorLocation(FVector(-1280.f, -200.f, 372.f));
		}
		else
		{
			Ball->SetActorLocation(FVector(-1280.f, 200.f, 372.f));
		}
	}
	else
	{
		if ((ptsTeam1 + ptsTeam2) % 2 == 1)
		{
			Ball->SetActorLocation(FVector(1280.f, 200.f, 372.f));
		}
		else
		{
			Ball->SetActorLocation(FVector(1280.f, -200.f, 372.f));
		}
	}

	// Resets ball location and rotation
	Ball->TennisBallMesh->SetSimulatePhysics(false); 
	Ball->TennisBallMesh->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
	Ball->TennisBallMesh->SetVisibility(false);

	ShowScore();
}
