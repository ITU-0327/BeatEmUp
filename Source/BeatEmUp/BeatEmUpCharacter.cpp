// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpCharacter.h"

#include "Enemy.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GrapplingHook.h"
#include "InputActionValue.h"
#include "Interactable.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABeatEmUpCharacter

ABeatEmUpCharacter::ABeatEmUpCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABeatEmUpCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(!bIsGrappling) return;

	FVector Direction = (GrapplingHookTarget - GetActorLocation()).GetSafeNormal();
	GrapplingForce += Direction * GrapplingHookAcceleration * DeltaTime;
	GrapplingForce = GrapplingForce.GetClampedToMaxSize(MaxGrapplingSpeed);

	GetCharacterMovement()->Velocity = GrapplingForce;

	if(FVector::Dist(GetActorLocation(), GrapplingHookTarget) < 100.f)
		StopGrapplingHook();
}

void ABeatEmUpCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ABeatEmUpCharacter::Punch() {
	if(!bPunchReady) return;

	bPunchReady = false;
	GetWorld()->GetTimerManager().SetTimer(PunchTimerHandle, this, &ABeatEmUpCharacter::ResetPunch, PunchCooldown, false);

	TArray<FHitResult> HitResults;
	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * PunchDistance;
	const FCollisionShape CubeShape = FCollisionShape::MakeBox(FVector(PunchDistance));
	GetWorld()->SweepMultiByChannel(HitResults, End, End, GetActorQuat(), ECC_WorldDynamic, CubeShape);

	TArray<AActor*> HitThisPunch;

	for(FHitResult HitResult : HitResults) {
		if(HitResult.GetActor() == this) continue;
		if(HitThisPunch.Contains(HitResult.GetActor())) continue;

		HitThisPunch.Add(HitResult.GetActor());
		AEnemy* HitEnemy = Cast<AEnemy>(HitResult.GetActor());
		if(HitEnemy == nullptr) continue;

		HitEnemy->Ragdoll();
		FVector LaunchDirection = HitEnemy->GetActorLocation() - GetActorLocation();
		LaunchDirection.Normalize();
		LaunchDirection *= 3;
		LaunchDirection += FVector::UpVector;
		HitEnemy->GetMesh()->AddImpulse(LaunchDirection * PunchForce);
		HitEnemy->DealDamage(PunchDamage);
	}
}

void ABeatEmUpCharacter::ResetPunch() {
	bPunchReady = true;
}

void ABeatEmUpCharacter::Use() {
	FVector Start = GetActorLocation();
	FVector End = Start + GetFollowCamera()->GetForwardVector() * UseDistance;

	FHitResult HitData;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActors(ActorsToIgnore);
	TraceParams.TraceTag = FName("Use Trace Tag");
	GetWorld()->DebugDrawTraceTag = TraceParams.TraceTag;

	bool bSweep = GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECC_Visibility, TraceParams);

	if(!bSweep) {
		UE_LOG(LogTemp, Warning, TEXT("Didn't hit an actor!"));
		return;
	}
	if(!HitData.GetActor()) {
		UE_LOG(LogTemp, Warning, TEXT("What happened!?"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("We hit: %s"), *HitData.GetActor()->GetName());
	
	if(HitData.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		IInteractable::Execute_Interact(HitData.GetActor());
	else
		UE_LOG(LogTemp, Warning, TEXT("Actor not interactable!"));
}

void ABeatEmUpCharacter::DealDamage(float Damage) {
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABeatEmUpCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABeatEmUpCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABeatEmUpCharacter::Look);

		// Punching
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::Punch);

		// Using
		EnhancedInputComponent->BindAction(UseAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::Use);

		// Grappling
		EnhancedInputComponent->BindAction(GrapplingAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::LaunchGrapplingHook);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABeatEmUpCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABeatEmUpCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABeatEmUpCharacter::LaunchGrapplingHook() {
	if(bIsGrapplingHookActive) {
		UE_LOG(LogTemp, Warning, TEXT("Already Activated!"));
		return;
	}
	
	FVector Start = GetActorLocation();
	FVector End = Start + GetFollowCamera()->GetForwardVector() * VisionDistance;

	FHitResult HitData;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECC_Visibility, TraceParams);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0, 1.0f);

	if(!bHit) return;
	if(HitData.GetActor() == nullptr) return;

	DrawDebugSphere(GetWorld(), HitData.ImpactPoint, 10.0f, 12, FColor::Red, false, 5.0f, 0, 1.0f);

	FVector Direction = (HitData.ImpactPoint - Start).GetSafeNormal();
	FRotator LaunchRotation = Direction.Rotation() + FRotator(-90.f, 0.f, 0.f);

	if (AGrapplingHook* GrapplingHook = GetWorld()->SpawnActor<AGrapplingHook>(GrapplingHookClass, Start, LaunchRotation)) {
		GrapplingHook->Launch(HitData.ImpactPoint, this);
		bIsGrapplingHookActive = true;
	}
}

void ABeatEmUpCharacter::StartGrapplingHook(const FVector& TargetLocation) {
	if(bIsGrappling) return;

	GrapplingHookTarget = TargetLocation;
	GrapplingForce = GetCharacterMovement()->Velocity;
	bIsGrappling = true;

	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void ABeatEmUpCharacter::StopGrapplingHook() {
	if(!bIsGrappling) return;

	bIsGrappling = false;
	bIsGrapplingHookActive = false;

	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}
