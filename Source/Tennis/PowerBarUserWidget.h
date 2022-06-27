// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TennisCharacter.h"
#include "PowerBarUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TENNIS_API UPowerBarUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual bool Initialize();
public:

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	TWeakObjectPtr<ATennisCharacter> TennisCharacterOwner;

	/** Judge msg on screen */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* ProgressPowerBar;

	void SetOwnerTennisCharacter(ATennisCharacter* InTennisCharacter);

	UFUNCTION(BlueprintCallable)
	void SetProgressPowerBar(float powerBarPercent);
};
