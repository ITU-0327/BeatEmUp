// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GrapplingHook.h"
#include "Grenade.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "BeatEmUpCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABeatEmUpCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Punch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PunchAction;

	/** Use Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UseAction;

	/** Grappling Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* GrapplingAction;

	/** Grappling Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PickUpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrowAction;
	
public:
	ABeatEmUpCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	bool bPunchReady = true;
	FTimerHandle PunchTimerHandle;
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Development
	UPROPERTY(EditAnywhere, Category="Development Settings")
		bool bEnableDebug = true;

	// Health
	UPROPERTY(EditAnywhere, Category="Character Settings")
		float MaxHealth = 100;
	UPROPERTY(EditAnywhere)
		int CurrentHealth = MaxHealth;
	
	// Punch
	UPROPERTY(EditAnywhere, Category="Punch Settings")
		float PunchDistance = 250;
	UPROPERTY(EditAnywhere, Category="Punch Settings")
		float PunchForce = 50000;
	UPROPERTY(EditAnywhere, Category="Punch Settings")
		float PunchDamage = 50;
	UPROPERTY(EditAnywhere, Category="Punch Settings")
		float PunchCooldown = 1;
	UFUNCTION()
		void Punch();
	UFUNCTION()
		void ResetPunch();

	// Use
	UPROPERTY(EditAnywhere, Category="Character Settings")
		float UseDistance = 1000;
	UFUNCTION()
		void Use();

	// Grappling Hook
	UPROPERTY(EditAnywhere, Category="Grappling Settings")
		TSubclassOf<AGrapplingHook> GrapplingHookClass;
	UPROPERTY(EditAnywhere, Category="Grappling Settings")
		float VisionDistance = 3000;
	UPROPERTY(EditAnywhere, Category="Grappling Settings")
		float PullForceStrength = 250000.f;
	UPROPERTY(EditAnywhere, Category="Grappling Settings")
		float GrapplingHookAcceleration = 2000.0f;
	UPROPERTY(EditAnywhere, Category="Grappling Settings")
		float MaxGrapplingSpeed = 2000.0f;
	
	FVector GrapplingHookTarget;
	FVector GrapplingForce;
	bool bIsGrappling = false;
	bool bIsGrapplingHookActive = false;
	
	UFUNCTION()
		void LaunchGrapplingHook();
	void StartGrapplingHook(const FVector& TargetLocation);
	void StopGrapplingHook();

	// Grenade
	UPROPERTY(EditAnywhere, Category="Grappling Settings")
		TSubclassOf<AGrenade> GrenadeClass;
	UPROPERTY(EditAnywhere, Category="Grenade Settings")
		float ThrowForce = 1000.f;
	bool bHasGrenade = false;
	void PickUpGrenade();
	void ThrowGrenade();
	
	void DealDamage(float Damage);

	virtual void Tick(float DeltaTime) override;
};
