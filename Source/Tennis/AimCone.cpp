// Fill out your copyright notice in the Description page of Project Settings.


#include "AimCone.h"

// Sets default values
AAimCone::AAimCone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>("ConeMesh");
	SetRootComponent(ConeMesh);
	ConeMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AAimCone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAimCone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

