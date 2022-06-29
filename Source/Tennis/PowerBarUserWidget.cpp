// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerBarUserWidget.h"
#include "Components/ProgressBar.h"
#include "Delegates/Delegate.h"
#include "TennisCharacter.h"
#include "GenericPlatform/GenericPlatform.h"
#include "UObject/WeakObjectPtrTemplates.h"

bool UPowerBarUserWidget::Initialize()
{
	Super::Initialize();
	ProgressPowerBar->Percent = 0.f;
	return true;
}

void UPowerBarUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!TennisCharacterOwner.IsValid())
		return;
}

void UPowerBarUserWidget::SetOwnerTennisCharacter(ATennisCharacter* InTennisCharacter)
{
	TennisCharacterOwner = InTennisCharacter;
}

float UPowerBarUserWidget::GetProgressPowerBar()
{
	return ProgressPowerBar->Percent;
}

void UPowerBarUserWidget::SetProgressPowerBar(float powerBarPercent)
{
	ProgressPowerBar->SetPercent(powerBarPercent);
}
