// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TennisBall.generated.h"

UCLASS()
class TENNIS_API ATennisBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATennisBall();

	// Moves ball with force and rotation returns location for animations
	FVector Hit(FVector force, FRotator rotation);

	UPROPERTY(EditAnywhere,Category = "Components")
	class UStaticMeshComponent* TennisBallMesh;

	// Cooldown for times floor is hit;
	bool CTimesFloorIsHit;

	// Times floor is hit
	int timesFloorIsHit;

	// Last field ball hit
	FString lastFieldHit;

	// Last field ball hit while player is serving
	FString lastServingFieldHit;

	// Number of times ball hit floor in point
	int timesBallHitFloor;

	// Times ball hit serving field
	int timesBallHitServing;

	// Times you can miss service
	int servingError;

	// Rotation of team that last hit ball
	int lastHit;

	// Rotation of team that is serving
	int servingTeam;

	// Resets all values for new point
	void ResetValues(bool resetServingError);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
