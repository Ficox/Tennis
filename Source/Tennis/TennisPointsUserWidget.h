// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TennisPointsUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FshowMSG);
/**
 * 
 */
UCLASS()
class TENNIS_API UTennisPointsUserWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize();

public:
	
	/** Sets all values to widget */
	void SetValues(int nSetsTeam1, int nSetsTeam2, int nGemsTeam1, int nGemsTeam2, FString nptsTeam1, FString nptsTeam2, int nServingTeam);

	/** Custome event for show msg */
	UPROPERTY(BlueprintAssignable)
	FshowMSG customEvent;

	/** Custome event for show judge msg */
	UPROPERTY(BlueprintAssignable)
	FshowMSG judgeMSGEvent;

	/** Show custome msg on screen */
	UFUNCTION(BlueprintCallable)
	void ShowMSG(FString outMsg);

	/** Show custome msg on screen */
	UFUNCTION(BlueprintCallable)
	void ShowJudgeMSG(FString outMsg);

	/** Judge msg on screen */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* judgeMSG;

	/** Popup msg on screen */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* msgOnScreen;

	/** Show number of sets team1 */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* SetsTeam1 = nullptr;

	/** Show number of sets team2 */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* SetsTeam2;

	/** Show number of gems team1 */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* GemsTeam1;

	/** Show number of gems team2 */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* GemsTeam2;

	/** Show number of points team1 */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* ptsTeam1;

	/** Show number of points team2 */
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* ptsTeam2;

	/** Rotation of serving team */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	int servingTeam;




};