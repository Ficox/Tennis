// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TennisCharacter.generated.h"

UCLASS(config=Game)
class ATennisCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;



	
public:
	ATennisCharacter( const FObjectInitializer& ObjectInitializer);

	int playerID;
	bool canShoot;
	bool isShootingTopSpin;
	bool isShootingSlice;
	bool isShootingLob;
	bool isBallScanned;

	// Used for animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsShooting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool isLob;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool isTopSpin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool isSlice;

	/** Location where ball should be */
	FVector ballLocation;

	/** When character started shooting */
	FDateTime startedShooting;

	/** When character finished shooting */
	FDateTime finishedShooting;

	/** Is player holding shot button */
	bool shotInput;

	/** Is character serving */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	bool isServing;
	
	/** Current stroke animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FString strokeAnimation;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "TVCamera")
	TSubclassOf<class ACameraActor> TVCamera;

	UPROPERTY(EditAnywhere, Category = "TVCamera")
	class ACameraActor* RealTVCamera;

	UPROPERTY(EditAnywhere, Category = "Shooting")
	TSubclassOf<class ATennisBall> TennisBallBP;

	UPROPERTY(EditAnywhere, Category = "Shooting")
	class AAimCone* Aim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball")
	class ATennisBall* TennisBall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* PowerBarWC;

	class UPowerBarUserWidget* PowerBar;


protected:

	void BeginPlay();

	/** Resets aim up */
	void ResetAimUp(float Value);

	/** Rests aim right */
	void ResetAimRight(float Value);
	
	/** Hits ball*/
	void HitBall(float strength, float CheckUpBoost, float ZUpBoost, float CheckUpDistance, FRotator BallRotation, float netHight);

	/** Hits ball with topSpin */
	void TopSpin();
	
	/** Resets ball */
	void ResetBall();
	
	/** Hits ball with slice */
	void Slice();

	/** Hits lob */
	void Lob();

	/** Find animation for right stroke*/
	void FindRightAnimation(FVector locationOfHit);

	/** Preparing to hit ball with TopSpin */
	void ShootingTopSpin();

	/** Preparing to hit ball with Slice */
	void ShootingSlice();

	/** Preparing to hit ball with  Lob*/
	void ShootingLob();

	/** Server hits ball */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Hit(FVector finalVector, FRotator rotation);
	
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	//Tick
	virtual void Tick(float DeltaSeconds) override;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

