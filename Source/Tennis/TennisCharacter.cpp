// Copyright Epic Games, Inc. All Rights Reserved.

#include "TennisCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Pawn.h"
#include "kismet/GameplayStatics.h"
#include <Tennis/TennisBall.h>
#include <Camera/CameraActor.h>
#include "MyCameraActor.h"
#include <string>
#include "Components/StaticMeshComponent.h"
#include "AimCone.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "TennisGameMode.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/WidgetComponent.h"
#include "PowerBarUserWidget.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// ATennisCharacter

ATennisCharacter::ATennisCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Is charater shooting
	IsShooting = false;

	// Is character able to shoot
	canShoot = true;

	// Is character serving
	isServing = false;

	// is ball scanned
	isBallScanned = false;
	
	// Is stroke active
	isLob = false;
	isTopSpin = false;
	isSlice = false;

	// shooting topspin/slice/lob
	isShootingLob = false;
	isShootingTopSpin = false;
	isShootingSlice = false;

	// Current stroke animation
	strokeAnimation = "None";

	// PlayerID
	playerID = 0;
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	PowerBarWC = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("PowerBar"));
	PowerBarWC->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//PowerBar = Cast<UPowerBarUserWidget>(PowerBarWC->GetUserWidgetObject());
	//PowerBar->SetOwnerTennisCharacter(this);
	// Attach TVCamera
	//RealTVCamera = CreateDefaultSubobject<AActor>(TEXT("TVCamera"));
	
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATennisCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent)

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATennisCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATennisCharacter::MoveRight);
	// Reset aim
	PlayerInputComponent->BindAxis("MoveForward", this, &ATennisCharacter::ResetAimUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATennisCharacter::ResetAimRight);


	// Hits ball with TopSpin
	PlayerInputComponent->BindAction("TopSpin", IE_Pressed, this, &ATennisCharacter::ShootingTopSpin);
    //PlayerInputComponent->BindAction("TopSpin", IE_Released, this, &ATennisCharacter::TopSpin);

	// Hits ball with Slice
	PlayerInputComponent->BindAction("Slice", IE_Pressed, this, &ATennisCharacter::ShootingSlice);
	//PlayerInputComponent->BindAction("Slice", IE_Released, this, &ATennisCharacter::Slice);

	// Hits ball with Lob
	PlayerInputComponent->BindAction("Lob", IE_Pressed, this, &ATennisCharacter::ShootingLob);
	//PlayerInputComponent->BindAction("Lob", IE_Released, this, &ATennisCharacter::Lob);

	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &ATennisCharacter::ResetBall);


// We have 2 versions of the rotation bindings to handle different kinds of devices differently
// "turn" handles devices that provide an absolute delta, such as a mouse.
// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
/*
PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
PlayerInputComponent->BindAxis("TurnRate", this, &ATennisCharacter::TurnAtRate);
PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
PlayerInputComponent->BindAxis("LookUpRate", this, &ATennisCharacter::LookUpAtRate);
*/
// handle touch devices
PlayerInputComponent->BindTouch(IE_Pressed, this, &ATennisCharacter::TouchStarted);
PlayerInputComponent->BindTouch(IE_Released, this, &ATennisCharacter::TouchStopped);

// VR headset functionality
PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATennisCharacter::OnResetVR);
}

void ATennisCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (PowerBarWC)
	{
		PowerBar = Cast<UPowerBarUserWidget>(PowerBarWC->GetUserWidgetObject());
		PowerBar->SetOwnerTennisCharacter(this);
	}

}

void ATennisCharacter::ResetAimUp(float Value)
{
	if (Value == 0.0f && Aim != NULL && !IsShooting)
	{
		FVector AimLocation = Aim->GetActorLocation();
		if (AimLocation.X != playerID * 564.25f)
		{
			FVector normalLocation = FVector(playerID * 564.25f, AimLocation.Y, AimLocation.Z);
			Aim->SetActorLocation(normalLocation);
		}
	}
}

void ATennisCharacter::ResetAimRight(float Value)
{
	if (Value == 0.0f && Aim != NULL && !IsShooting)
	{
		FVector AimLocation = Aim->GetActorLocation();
		if (AimLocation.Y != 0.f)
		{
			FVector normalLocation = FVector(AimLocation.X, 0.f, AimLocation.Z);
			Aim->SetActorLocation(normalLocation);
		}
	}
}

void ATennisCharacter::HitBall(float strength, float CheckUpBoost, float ZUpBoost, float CheckUpDistance, FRotator BallRotation, float netHight = 240.f)
{
	canShoot = false;
	FVector TennisBallLocation = TennisBall->GetActorLocation();
	FVector dir = (Aim->GetActorLocation() - TennisBallLocation).GetSafeNormal();
	// Difference between balls hight and nets hight
	FVector upBoost = FVector(0.f, 0.f, netHight - TennisBallLocation.Z);
	if (upBoost.Z < CheckUpBoost)
	{
		upBoost.Z = CheckUpBoost;
	}

	// Force by distance
	FVector AimLocation = Aim->GetActorLocation();
	// how strong is shot
	float power = PowerBar->GetProgressPowerBar();
	if (power < 0) {
		power = 0.01;
	}
	// how far is character
	float distance = (AimLocation - TennisBallLocation).Size();
	if (distance > CheckUpDistance)
	{
		distance = CheckUpDistance;
	}
	float force = distance / CheckUpDistance;
	// bonus power
	float bonusPower = 300.f;
	strength += bonusPower * power - 300.f;
	ZUpBoost += bonusPower * 0.3f * (1.f - power);
	// Up force
	FVector upVector = FVector(0.f, 0.f, ZUpBoost) + upBoost;
	// how far from net player is
	if (((abs(GetActorLocation().X) < 400.f && TennisBallLocation.Z > 250.f) || TennisBallLocation.Z > 340.f) && !isServing)
	{
		upVector += FVector(0.f, 0.f, -(TennisBallLocation.Z - netHight) * 2);
		strength += 250.f;
	}

	

	// Adding ball rotation
	FVector finalVector = dir * strength * force + upVector;

	// Additional vectors for forehand and backhand
	FVector backhandVector = FVector(20.f, -70.f, 25.f * playerID);
	FVector forehandVector = FVector(20.f, 70.f, 25.f * playerID);
	
	// Is ball in range
	if ((((GetActorLocation() + backhandVector * playerID) - TennisBallLocation).Size() < 150.f || ((GetActorLocation() + forehandVector * playerID) - TennisBallLocation).Size() < 150.f) && (TennisBall->timesBallHitFloor > 0 || isServing))
	{


		PowerBar->SetProgressPowerBar(power);
		
		//finishedShooting = FDateTime::Now();
		FTimespan secondsShooting = startedShooting - finishedShooting;
		//PowerBar->SetProgressPowerBar(90.f);
		TennisBall->lastHit = playerID;
		TennisBall->timesFloorIsHit = 0;
		TennisBall->lastFieldHit = "";
		TennisBall->lastServingFieldHit = "";
		//PowerBar->SetProgressPowerBar(0.f);
		if (HasAuthority())
		{
			FVector locationOfHit = TennisBall->Hit(finalVector, BallRotation * playerID);
			FindRightAnimation(locationOfHit);
		}
		else
		{
			Server_Hit(finalVector, BallRotation * playerID);
		}
			
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
				// reset all
				isShootingLob = false;
				isShootingTopSpin = false;
				isShootingSlice = false;
				IsShooting = false;
				if (isServing) {
					isServing = false;
				}
				isLob = false;
				isTopSpin = false;
				isSlice = false;
				PowerBar->SetProgressPowerBar(0.f);
				canShoot = true;
				isBallScanned = false;
			}, 0.3, false);
		
	}
	else
	{
		FTimerHandle TimerHandle2;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle2, [&]()
		// reset all
		{
			isShootingLob = false;
			isShootingTopSpin = false;
			isShootingSlice = false;
			IsShooting = false;
			isLob = false;
			isTopSpin = false;
			isSlice = false;
			PowerBar->SetProgressPowerBar(0.f);
			canShoot = true;
			isBallScanned = false;
		}, 0.5f, false);
		
	}
	
}

void ATennisCharacter::TopSpin()
{
	// Point system
	/*
	ATennisGameMode* tgm = (ATennisGameMode*)UGameplayStatics::GetGameMode(GetWorld());
	if (playerID > 0)
	{
		tgm->AddPoint(1);
		tgm->ShowScore();
	}
	else
	{
		tgm->AddPoint(2);
		tgm->ShowScore();
	}
	*/
	if (canShoot) 
	{
		isTopSpin = true;
		HitBall(1000, 30.f, 220.f, 2000.f, FRotator(0.f, 15.f, 0.f),300.f);
	}

}

void ATennisCharacter::ResetBall()
{
	TennisBall->SetActorLocation(GetActorLocation() + FVector(0.f, 30.f, 120.f));
	TennisBall->TennisBallMesh->SetSimulatePhysics(false);
}

void ATennisCharacter::Slice()
{
	// Spawn Actor
	/*
	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.SetLocation(FollowCamera->GetComponentRotation().Vector() * 200.f + GetActorLocation());
	SpawnTransform.TransformPosition(FVector(0.f, 0.f, 200.f));
	FActorSpawnParameters SpawnParams;
	FVector Forward = this->GetActorForwardVector();
	ATennisBall* ball = (ATennisBall*)GetWorld()->SpawnActor<ATennisBall>(TennisBallBP, SpawnTransform, SpawnParams);
	*/
	if (canShoot)
	{
		isSlice = true;
		HitBall(780.f, 20.f, 320.f, 1800.f, FRotator(0.f, 15.f, 0.f));
	}
}
void ATennisCharacter::Lob()
{
	if (canShoot)
	{
		isLob = true;
		HitBall(600.f, 20.f, 450.f, 1800.f, FRotator(0.f, 15.f, 0.f));
	}
}

void ATennisCharacter::FindRightAnimation(FVector locationOfHit)
{
	FString outString;
	TSubclassOf<ATennisCharacter> classToFind;
	classToFind = ATennisCharacter::StaticClass();
	TArray<AActor*> foundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundPlayers);
	FVector playerLocation;
	int playerRotation;
	int id;
	for (size_t i = 0; i < foundPlayers.Num(); i++)
	{
		if (((ATennisCharacter*)foundPlayers[i])->playerID != playerID) 
		{
			playerLocation = ((ATennisCharacter*)foundPlayers[i])->GetTargetLocation();
			playerRotation = ((ATennisCharacter*)foundPlayers[i])->playerID;
id = i;
((ATennisCharacter*)foundPlayers[i])->ballLocation = locationOfHit;
		}
	}
	FVector direction = (playerLocation - locationOfHit).GetSafeNormal();
	if (direction.Y * playerRotation < 0)
	{
		outString += "Forehand";
	}
	else {
	outString += "Backhand";
	}
	((ATennisCharacter*)foundPlayers[id])->strokeAnimation = outString;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, outString);
}



void ATennisCharacter::ShootingTopSpin()
{
	if (!IsShooting)
	{
		startedShooting = FDateTime::Now();
		finishedShooting = startedShooting + FTimespan().FromMilliseconds(1200);
	}
	IsShooting = true;
	isShootingTopSpin = true;

}
void ATennisCharacter::ShootingSlice()
{
	if (!IsShooting)
	{
		startedShooting = FDateTime::Now();
		finishedShooting = startedShooting + FTimespan().FromMilliseconds(1200);
	}
	IsShooting = true;
	isShootingSlice = true;

}
void ATennisCharacter::ShootingLob()
{
	if (!IsShooting)
	{
		startedShooting = FDateTime::Now();
		finishedShooting = startedShooting + FTimespan().FromMilliseconds(1200);
	}
	IsShooting = true;
	isShootingLob = true;

}


bool ATennisCharacter::Server_Hit_Validate(FVector finalVector, FRotator rotation)
{
	return true;
}

void ATennisCharacter::Server_Hit_Implementation(FVector finalVector, FRotator rotation)
{

	FVector locationOfHit = TennisBall->Hit(finalVector, rotation);
	FindRightAnimation(locationOfHit);
}

void ATennisCharacter::OnResetVR()
{
	// If Tennis is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Tennis.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATennisCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATennisCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ATennisCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// sets player rotation
	if (playerID == 0)
	{
		if (GetActorRotation().Yaw < 0.f)
		{
			playerID = -1;
		}
		else
		{
			playerID = 1;
		}

	}

	if (TennisBall != nullptr)
	{

		// hit ball if it is close to character
		FVector TennisBallLocation = TennisBall->GetActorLocation();
		if ((GetActorLocation() - TennisBallLocation).Size() < 350.f && !isServing && canShoot && IsShooting && (isShootingTopSpin || isShootingSlice || isShootingLob))
		{
			// scan ball
			if (!isBallScanned)
			{
				// get closer location
				FVector ballVelocity = TennisBall->TennisBallMesh->GetComponentVelocity();
				FVector scan1 = TennisBallLocation + ballVelocity * 0.5f;
				FVector scan2 = TennisBallLocation + ballVelocity * 0.2f;
				if ((GetActorLocation() - scan1).Size() < (GetActorLocation() - scan2).Size()+20.f)
				{
					ballLocation = scan1;
				}
				else
				{
					ballLocation = scan2;
				}
				
				isBallScanned = true;
			}
			// move player to hit ball
			FVector TennisCharacterLocation = GetActorLocation();
			FVector direction;
			FVector backhandVector = FVector(40.f, -70.f, 40.f * playerID);
			FVector forehandVector = FVector(40.f, 70.f, 40.f * playerID);
			if (strokeAnimation.Equals("Forehand"))
			{
				direction = (ballLocation - TennisCharacterLocation + backhandVector * playerID).GetSafeNormal();
			}
			else
			{
				direction = (ballLocation - TennisCharacterLocation + forehandVector * playerID).GetSafeNormal();
			}
			// auto move
			if ((ballLocation - TennisCharacterLocation).Size() > 30)
			{
				AddMovementInput(direction, 15.f);
			}
			
		}
		if ((GetActorLocation() - TennisBallLocation).Size() < 140.f && canShoot && IsShooting && !isServing)
		{
			if (isShootingTopSpin)
			{
				TopSpin();
			}
			if (isShootingSlice)
			{
				Slice();
			}
			if (isShootingLob)
			{
				Lob();
			}
			
			
		}
		// serve 
		if (isServing && canShoot && IsShooting)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
				// serve after 1 sec
				{
					if (isShootingTopSpin)
					{
						TopSpin();
					}
					if (isShootingSlice)
					{
						Slice();
					}
					if (isShootingLob)
					{
						Lob();
					}
				}, 1.f, false);
			


		}
		
		// change animation if needed

		/*
		if ((GetActorLocation() - TennisBallLocation).Size() < 50.f)
		{
			if (((GetActorLocation() + backhandVector * playerID) - TennisBallLocation).Size() > ((GetActorLocation() + forehandVector * playerID) - TennisBallLocation).Size())
			{
				strokeAnimation = "Forehand";
			}
			else
			{
				strokeAnimation = "Backhand";
			}
		}
		*/
	}

	// Move to ball
	if (ballLocation != FVector(0.f, 0.f, 0.f) && IsShooting)
	{
		FVector TennisCharacterLocation = GetActorLocation();
		FVector direction;
		FVector backhandVector = FVector(140.f, -70.f, 40.f * playerID);
		FVector forehandVector = FVector(140.f, 70.f, 40.f * playerID);
		if (strokeAnimation.Equals("Forehand")) 
		{
			direction = (ballLocation - TennisCharacterLocation + backhandVector * playerID).GetSafeNormal();
		}
		else
		{
			direction = (ballLocation - TennisCharacterLocation + forehandVector * playerID).GetSafeNormal();
		}
		//TennisCharacterLocation += direction * 30 * DeltaSeconds;
		// auto move
		if (!((ballLocation - TennisCharacterLocation + backhandVector * playerID).Size() < 50) || !((ballLocation - TennisCharacterLocation + forehandVector * playerID).Size() < 50))
		{
			//AddMovementInput(direction, 1.f);
		}
		
		//SetActorLocation(FVector(TennisCharacterLocation.X, TennisCharacterLocation.Y, GetActorLocation().Z));
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), FVector(ballLocation.X,ballLocation.Y,TennisCharacterLocation.Z));		
	}

	// Sets ball above player
	if (TennisBall && isServing && !IsShooting) 
	{
		FVector playerLocation = GetTargetLocation();
		TennisBall->SetActorLocation(FVector(playerLocation.X, playerLocation.Y, TennisBall->GetTargetLocation().Z));

	}

	// Sets power bar
	if (PowerBar && IsShooting)
	{
		FTimespan currentTime = FDateTime::Now() - finishedShooting;
		PowerBar->SetProgressPowerBar((1200.f - abs(currentTime.GetTotalMilliseconds())) / 1200.f);
	}

	// Finds and attaches TennisBall if there is none
	if (TennisBall == NULL)
	{
		TSubclassOf<ATennisBall> classToFind;
		classToFind = ATennisBall::StaticClass();
		TArray<AActor*> foundTennisBalls;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundTennisBalls);
		if(HasAuthority()&& (foundTennisBalls.Num() == 0))
		{
			if(UWorld * const World = GetWorld())
			{
				FTransform SpawnTransform = GetActorTransform();
				SpawnTransform.SetLocation(FollowCamera->GetComponentRotation().Vector() * 20.f + GetActorLocation());
				SpawnTransform.TransformPosition(FVector(0.f, 0.f, 280.f));
				FActorSpawnParameters SpawnParams;
				FVector Forward = this->GetActorForwardVector();
				World->SpawnActor<ATennisBall>(TennisBallBP, SpawnTransform, SpawnParams);
				GetController()->SetControlRotation(FRotator(0.f, -180.f,0.f));
				playerID = -1;
				ATennisGameMode* tgm = (ATennisGameMode*)UGameplayStatics::GetGameMode(GetWorld());
				tgm->ResetAll();
			}
		}
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundTennisBalls);
		TennisBall = (ATennisBall*)foundTennisBalls.Pop();
	}
	// Finds and creates AimCone if there is none
	if (Aim == NULL)
	{
		TSubclassOf<AAimCone> classToFind;
		classToFind = AAimCone::StaticClass();
		TArray<AActor*> foundTennisBalls;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundTennisBalls);
		if (UWorld* const World = GetWorld())
		{
			FTransform SpawnTransform = GetActorTransform();
			SpawnTransform.SetLocation(FollowCamera->GetComponentRotation().Vector() * 200.f + GetActorLocation());
			SpawnTransform.TransformPosition(FVector(0.f, 0.f, 200.f));
			FActorSpawnParameters SpawnParams;
			FVector Forward = this->GetActorForwardVector();
			Aim = World->SpawnActor<AAimCone>(classToFind, SpawnTransform, SpawnParams);
		}
		//Aim = (AAimCone*)foundTennisBalls.Pop();
	}
	// switch to TVCamera
	APlayerController* OurPlayer1 = UGameplayStatics::GetPlayerController(this, 0);
	//APlayerController* OurPlayer2 = UGameplayStatics::GetPlayerController(this, 1);
	//if (OurPlayer2) {

	//	OurPlayer2->Possess(this);

	//}
	if (OurPlayer1)
	{
		if (RealTVCamera == NULL)
		{
			// Finds the camera
			TSubclassOf<ACameraActor> classToFind;
			classToFind = ACameraActor::StaticClass();
			TArray<AActor*> foundCameras;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundCameras);
			bool cameraFound = false;

			for (AActor* camera : foundCameras)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, camera->GetActorLabel());
				if ( FString("TopCamera").Equals(camera->GetActorLabel()))
				{
					RealTVCamera = (ACameraActor*)camera;
					cameraFound = true;
				}
			}
			//UE_LOG(LogTemp, Warning, TEXT("Camers found: %i"), foundCameras.Num());

		}
		else
		{
			// Attaches the camera
			if ((OurPlayer1->GetViewTarget() != RealTVCamera))
			{
				UE_LOG(LogTemp, Warning, TEXT("OurPlayer1.2"));
				OurPlayer1->SetViewTargetWithBlend(RealTVCamera, 0.f);
			}
		}
					
		
	}
	
	
	
}

void ATennisCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATennisCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATennisCharacter::MoveForward(float Value)
{

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//FString IntAsString = FString::SanitizeFloat(Value);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, IntAsString);
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		if (!(IsShooting) && (!isServing))
		{
			AddMovementInput(Direction, Value * playerID);		
		}
		else
		{
			// find out which way is forward			
			if (!isServing && canShoot) 
			{
				AddMovementInput(Direction, 0.5f * Value * playerID);
			}
			if (playerID > 0) 
			{
				if (Value > 0.f)
				{
					// aim up
				
					FVector upVector = FVector(560.f, 0, 0);
					FVector AimLocation = Aim->GetActorLocation();
					if (AimLocation.X + 560.f < 1140.f)
					{
						Aim->SetActorLocation(Aim->GetActorLocation() + upVector);
					}

				}
			
				else
				{
					// aim down
					FVector downVector = FVector(-320.f, 0, 0);
					FVector AimLocation = Aim->GetActorLocation();
					if (AimLocation.X - 320.f > 244.f)
					{
						Aim->SetActorLocation(Aim->GetActorLocation() + downVector);
					}
				}
			}
			else
			{
				if (Value > 0.f)
				{
					// aim up

					FVector upVector = FVector(320.f, 0, 0);
					FVector AimLocation = Aim->GetActorLocation();
					if (AimLocation.X + 320.f < -244.f)
					{
						Aim->SetActorLocation(Aim->GetActorLocation() + upVector);
					}

				}

				else
				{
					// aim down
					FVector downVector = FVector(-560, 0, 0);
					FVector AimLocation = Aim->GetActorLocation();
					if (AimLocation.X - 560 >-1140.f)
					{
						Aim->SetActorLocation(Aim->GetActorLocation() + downVector);
					}
				}
			}
		}

	}
}

void ATennisCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		if (!(IsShooting) && (!isServing))
		{
			/* not good
			if (Value * playerID > 0) {
				strokeAnimation = "Forehand";
			}
			else
			{
				strokeAnimation = "Backhand";
			}
			*/
			AddMovementInput(Direction, Value * playerID);
		}
		else if (isServing) 
		{
			// serving player is only moving on line
			FVector currentLocation = GetTargetLocation();
			if (currentLocation.Y  > 0.f)
			{
				if (Value  > 0.f  && currentLocation.Y < 400.f && !(IsShooting))
				{
					AddMovementInput(Direction, 0.20 * Value * playerID);
				}
				else if (Value  < 0.f  && currentLocation.Y > 50.f && !(IsShooting))
				{
					AddMovementInput(Direction, 0.20 * Value * playerID);
				}
			}
			else 
			{
				if (Value  < 0.f  && currentLocation.Y > -400.f && !(IsShooting))
				{
					AddMovementInput(Direction, 0.20 * Value * playerID);
				}
				else if (Value  > 0.f  && currentLocation.Y < -50.f && !(IsShooting))
				{
					AddMovementInput(Direction, 0.20 * Value * playerID);
				}
			}
			// aim while serving left/right
			if (Value > 0.f)
			{
				// aim right
				FVector rightVector = FVector(0.f, 380.f, 0.f);
				FVector AimLocation = Aim->GetActorLocation();
				if (AimLocation.Y + 380.f < 381.f)
				{
					Aim->SetActorLocation(Aim->GetActorLocation() + rightVector);
				}

			}
			else
			{
				// aim left
				FVector leftVector = FVector(0.f, -380.f, 0.f);
				FVector AimLocation = Aim->GetActorLocation();
				if (AimLocation.Y - 380.f > -381.f)
				{
					Aim->SetActorLocation(Aim->GetActorLocation() + leftVector);
				}
			}

		}
		else
		{
			if (canShoot)
			{
				AddMovementInput(Direction, 0.5f * Value * playerID);
			}
			if (Value > 0.f)
			{
				// aim right
				FVector rightVector = FVector(0.f, 380.f, 0.f);
				FVector AimLocation = Aim->GetActorLocation();
				if (AimLocation.Y + 380.f < 381.f)
				{
					Aim->SetActorLocation(Aim->GetActorLocation() + rightVector);
				}

			}
			else
			{
				// aim left
				FVector leftVector = FVector(0.f, -380.f, 0.f);
				FVector AimLocation = Aim->GetActorLocation();
				if (AimLocation.Y - 380.f > -381.f)
				{
					Aim->SetActorLocation(Aim->GetActorLocation() + leftVector);
				}
			}
		}
	}
}
