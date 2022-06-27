// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AimCone.generated.h"

UCLASS()
class TENNIS_API AAimCone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAimCone();
	// Sets cone mesh
	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* ConeMesh;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
