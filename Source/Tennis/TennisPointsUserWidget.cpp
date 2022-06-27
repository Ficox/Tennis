// Fill out your copyright notice in the Description page of Project Settings.


#include "TennisPointsUserWidget.h"
#include "Components/TextBlock.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "TennisGameMode.h"
#include "GenericPlatform/GenericPlatform.h"
#include "Delegates/Delegate.h"



bool UTennisPointsUserWidget::Initialize()
{
	Super::Initialize();
	ATennisGameMode* tgm = (ATennisGameMode*)UGameplayStatics::GetGameMode(GetWorld());
	if (tgm != nullptr) 
	{
		
		//SetValues(tgm->SetsTeam1, tgm->SetsTeam2, tgm->GemsTeam1, tgm->GemsTeam2, tgm->ptsTeam1, tgm->ptsTeam2);
		tgm->TennisUI = this;
	}
	return true;
}

void UTennisPointsUserWidget::SetValues(int nSetsTeam1, int nSetsTeam2, int nGemsTeam1, int nGemsTeam2, FString nptsTeam1, FString nptsTeam2, int nServingTeam)
{
	SetsTeam1->SetText(FText::FromString(FString::FromInt(nSetsTeam1)));
	SetsTeam2->SetText(FText::FromString(FString::FromInt(nSetsTeam2)));
	GemsTeam1->SetText(FText::FromString(FString::FromInt(nGemsTeam1)));
	GemsTeam2->SetText(FText::FromString(FString::FromInt(nGemsTeam2)));
	ptsTeam1->SetText(FText::FromString(nptsTeam1));
	ptsTeam2->SetText(FText::FromString(nptsTeam2));
	servingTeam = nServingTeam;

}

void UTennisPointsUserWidget::ShowMSG(FString outMsg)
{
	msgOnScreen->SetText(FText::FromString(outMsg));
	customEvent.Broadcast();
}

void UTennisPointsUserWidget::ShowJudgeMSG(FString outMsg)
{
	judgeMSG->SetText(FText::FromString(outMsg));
	judgeMSGEvent.Broadcast();
}


