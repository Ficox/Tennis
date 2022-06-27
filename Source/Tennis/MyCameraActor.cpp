// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCameraActor.h"
#include <Camera/CameraActor.h>

AMyCameraActor::AMyCameraActor()
{
	//Super::AActor;
	
}

void AMyCameraActor::MakeRealCamera()
{
	FVector loc = FVector(-3180.f, 0.f, 1430.f);
	FRotator rot = FRotator(-25.f, 0.f, 0.f);

	//RealTVCamera = (ACameraActor*)GetWorld()->SpawnActor<AActor>(ACameraActor::StaticClass(), loc, rot);

}
