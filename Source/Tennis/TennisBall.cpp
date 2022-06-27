// Fill out your copyright notice in the Description page of Project Settings.


#include "TennisBall.h"
#include "Components/StaticMeshComponent.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include <Net/UnrealNetwork.h>
#include <Tennis/TennisGameMode.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Particles/ParticleSystemComponent.h"
#include "TennisPointsUserWidget.h"

// Sets default values
ATennisBall::ATennisBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	TennisBallMesh = CreateDefaultSubobject<UStaticMeshComponent>("TennisBallMesh");
	//SetRootComponent(TennisBallMesh);
	bReplicates = true;
	CTimesFloorIsHit = true;
	TennisBallMesh->SetNotifyRigidBodyCollision(true);
	TennisBallMesh->SetSimulatePhysics(false);
	timesFloorIsHit = 0;
	timesBallHitFloor = 0;
	timesBallHitServing = 0;
	lastFieldHit = "";
	lastServingFieldHit = "";
	servingError = 1;
	TennisBallMesh->OnComponentHit.AddDynamic(this, &ATennisBall::OnHit);
	TennisBallMesh->OnComponentBeginOverlap.AddDynamic(this, &ATennisBall::OnOverlapBegin);
	TennisBallMesh->OnComponentEndOverlap.AddDynamic(this, &ATennisBall::OnOverlapEnd);
	
}

FVector ATennisBall::Hit(FVector force, FRotator rotation) {
	
	FVector noMovement = FVector(0.f, 0.f, 0.f);
	FRotator noRotation = FRotator(0.f, 0.f, 0.f);
	// Stop all movement
	TennisBallMesh->SetVisibility(true);
	TennisBallMesh->SetSimulatePhysics(false);
	TennisBallMesh->SetSimulatePhysics(true);
	TennisBallMesh->AddImpulse(force);
	TennisBallMesh->AddWorldRotation(rotation);
	FPredictProjectilePathParams predictParams;
	FPredictProjectilePathResult predictResult;
	TArray<AActor*> ignoreList;
	ignoreList.Add((AActor*)this);
	predictParams.ActorsToIgnore = ignoreList;
	predictParams.bTraceWithCollision = true;
	predictParams.TraceChannel = ECollisionChannel::ECC_PhysicsBody;
	predictParams.bTraceComplex = false;
	predictParams.StartLocation = GetTargetLocation();
	predictParams.LaunchVelocity = force;
	predictParams.DrawDebugTime = 1.f;
	predictParams.SimFrequency = 1.f;
	predictParams.MaxSimTime = 4.5f;
	predictParams.ProjectileRadius = 3.f;
	predictParams.OverrideGravityZ = -980.f * 0.24f;
	predictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	UGameplayStatics::PredictProjectilePath(GetWorld(), predictParams, predictResult);
	FVector location = predictResult.HitResult.Location;
	return location;
	
	//TennisBallMesh->MoveComponent(force, rotation,false);
}

// Called when the game starts or when spawned
void ATennisBall::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATennisBall::ResetValues(bool resetServingError = true)
{
	timesFloorIsHit = 0;
	timesBallHitFloor = 0;
    lastFieldHit = "";
	lastServingFieldHit= "";
	if (resetServingError)
	{
		servingError = 1;
	}
		
}

// Called every frame
void ATennisBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Did ball hit right serving field
	
	if (timesBallHitFloor == 1 && timesFloorIsHit == 1) {
		ATennisGameMode* tgm = (ATennisGameMode*)UGameplayStatics::GetGameMode(GetWorld());
		int pointSum = tgm->ptsTeam1 + tgm->ptsTeam2;
		if (servingTeam > 0 && !lastServingFieldHit.Equals("TopLeftServingField") && !lastFieldHit.Equals("") && pointSum % 2 == 0)
		{
			if (servingError < 1)
			{
				tgm->GetTennisUI()->ShowJudgeMSG("DOUBLE FAULT!");
				tgm->AddPoint(2);				
				tgm->ResetAll();
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(TopLeftServingField)"), );
			}
			else
			{
				tgm->GetTennisUI()->ShowJudgeMSG("FAULT!");
				servingError--;
				tgm->ResetAll();
				ResetValues(false);
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(TopLeftServingField)"), );
			}

		}
		if (servingTeam > 0 && !lastServingFieldHit.Equals("TopRightServingField") && !lastFieldHit.Equals("") && pointSum % 2 == 1)
		{
			if (servingError < 1)
			{
				tgm->GetTennisUI()->ShowJudgeMSG("DOUBLE FAULT!");
				tgm->AddPoint(2);				
				tgm->ResetAll();
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(TopRightServingField)"), );
			}
			else
			{
				tgm->GetTennisUI()->ShowJudgeMSG("FAULT!");
				servingError--;
				tgm->ResetAll();
				ResetValues(false);
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(TopRightServingField)"), );
			}
		}
		if (servingTeam < 0 && !lastServingFieldHit.Equals("BottomLeftServingField") && !lastFieldHit.Equals("") && pointSum % 2 == 1)
		{
			if (servingError < 1)
			{
				tgm->GetTennisUI()->ShowJudgeMSG("DOUBLE FAULT!");
				tgm->AddPoint(1);				
				tgm->ResetAll();
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(BottomLeftServingField)"), );
			}
			else
			{
				tgm->GetTennisUI()->ShowJudgeMSG("FAULT!");
				servingError--;
				tgm->ResetAll();
				ResetValues(false);
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(BottomLeftServingField)"), );
			}
		}
		if (servingTeam < 0 && !lastServingFieldHit.Equals("BottomRightServingField") && !lastFieldHit.Equals("") && pointSum % 2 == 0)
		{
			if (servingError < 1)
			{
				tgm->GetTennisUI()->ShowJudgeMSG("DOUBLE FAULT!");
				tgm->AddPoint(1);
				tgm->ResetAll();
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(BottomRightServingField)"), );
			}
			else
			{
				tgm->GetTennisUI()->ShowJudgeMSG("FAULT!");
				servingError--;
				tgm->ResetAll();
				ResetValues(false);
				UE_LOG(LogTemp, Warning, TEXT("!lastServingFieldHit.Equals(BottomRightServingField)"), );
			}
		}
	}
	if (timesFloorIsHit > 0 && timesBallHitFloor > 0)
		{
			ATennisGameMode* tgm = (ATennisGameMode*)UGameplayStatics::GetGameMode(GetWorld());
			// ball hit floor more then 1 time
			if (timesFloorIsHit == 2 && lastHit > 0)
			{
				tgm->AddPoint(1);				
				tgm->ResetAll();
				tgm->GetTennisUI()->ShowJudgeMSG("IN!");
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("timesFloorIsHit > 1 && lastHit > 0"), );
			}
			else if (timesFloorIsHit == 2 && lastHit < 0)
			{
				tgm->AddPoint(2);
				tgm->ResetAll();
				tgm->GetTennisUI()->ShowJudgeMSG("IN!");
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("timesFloorIsHit > 1 && lastHit < 0"), );
			}
			// player did not hit in right field
			if (timesFloorIsHit == 1 && !lastFieldHit.Equals("TopField") && !lastFieldHit.Equals("") && lastHit > 0)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString("OVDE!")+lastFieldHit);
				tgm->AddPoint(2);
				tgm->ResetAll();
				tgm->GetTennisUI()->ShowJudgeMSG("OUT!");
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("timesFloorIsHit == 1 && !lastFieldHit.Equals(TopField) && !lastFieldHit.Equals() && lastHit > 0"), );
			}
			else if (timesFloorIsHit == 1 && !lastFieldHit.Equals("BottomField") && !lastFieldHit.Equals("") && lastHit < 0)
			{
				tgm->AddPoint(1);
				tgm->ResetAll();
				tgm->GetTennisUI()->ShowJudgeMSG("OUT!");
				ResetValues();
				UE_LOG(LogTemp, Warning, TEXT("timesFloorIsHit == 1 && !lastFieldHit.Equals(BottomField) && !lastFieldHit.Equals() && lastHit < 0"), );
			}
		}
}

void ATennisBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ATennisBall, timesFloorIsHit)
}

void ATennisBall::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	FString outString = FString();
	outString = FString::FromInt(timesFloorIsHit);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString("Floor hit:")+outString);
	FString name = *OtherActor->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
	if(name.Equals(FString("Floor")) && CTimesFloorIsHit)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
		timesFloorIsHit++;
		CTimesFloorIsHit = false;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				CTimesFloorIsHit = true;
			}, 0.1, false);
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, outString);
		outString = FString::FromInt(timesFloorIsHit);
	}
}

void ATennisBall::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	FString outString = FString();
	outString = FString::FromInt(timesFloorIsHit);
	FString name = *OtherActor->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
	if (name.Equals(FString("TopField")) || name.Equals(FString("BottomField")) || name.Equals(FString("Out1")) || name.Equals(FString("Out5")) || name.Equals(FString("Out2")) || name.Equals(FString("Out3")) || name.Equals(FString("Out4")))
	{
		lastFieldHit = name;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
		timesBallHitFloor++;

	}
	if (name.Equals(FString("BottomLeftServingField")) || name.Equals(FString("BottomRightServingField")) || name.Equals(FString("TopLeftServingField")) || name.Equals(FString("TopRightServingField")))
	{
		lastServingFieldHit = name;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
	}
	*/
}

void ATennisBall::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
	FString outString = FString();
	outString = FString::FromInt(timesFloorIsHit);
	FString name = *OtherActor->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
	if (name.Equals(FString("TopField")) || name.Equals(FString("BottomField")) || name.Equals(FString("Out1")) || name.Equals(FString("Out5")) || name.Equals(FString("Out2")) || name.Equals(FString("Out3")) || name.Equals(FString("Out4")))
	{
		lastFieldHit = name;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
		timesBallHitFloor++;

	}
	if (name.Equals(FString("BottomLeftServingField")) || name.Equals(FString("BottomRightServingField")) || name.Equals(FString("TopLeftServingField")) || name.Equals(FString("TopRightServingField")))
	{
		lastServingFieldHit = name;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, name);
	}
	
}
