// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpCharacter.h"

#include "BeatEmUpGameMode.h"
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
#include "Grenade.h"
#include "PauseMenu.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

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

	// Particle system for Trail Effect
	TeleportTrailComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Teleport Trail"));
	TeleportTrailComponent->SetupAttachment(RootComponent);
	TeleportTrailComponent->bAutoActivate = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABeatEmUpCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if(GetWorld()->GetTimerManager().TimerExists(PunchTimerHandle))
		InGameUI->UpdateValues();

	if(bIsGrapplingHookActive)
		UpdateGrapplingMaterial();

	if(bIsGrappling) {
		FVector Direction = (GrapplingHookTarget - GetActorLocation()).GetSafeNormal();
        GrapplingForce += Direction * GrapplingHookAcceleration * DeltaTime;
        GrapplingForce = GrapplingForce.GetClampedToMaxSize(MaxGrapplingSpeed);

		// Update the character's velocity with the grappling force
        GetCharacterMovement()->Velocity = GrapplingForce;

		// If the character is within 100 units of the target, stop grappling
        if(FVector::Dist(GetActorLocation(), GrapplingHookTarget) < 100.f) {
        	UE_LOG(LogTemp, Warning, TEXT("Stopping Grappling Hook! Character is close enough!"));
        	StopGrapplingHook();
        }
	}

	// Check if a portal creation process is ongoing and cancel it if the character moves too far away
	if (ActivePortalSystem && !ActivePortalSystem->ExitPortal && FVector::Dist(GetActorLocation(), ActivePortalSystem->EntryPortal->GetActorLocation()) > PortalRange) {
		EndPortalCreation();
		UE_LOG(LogTemp, Warning, TEXT("Character is too far from the portal!, the distance is %s"), *FString::SanitizeFloat(FVector::Dist(GetActorLocation(), ActivePortalSystem->EntryPortal->GetActorLocation())));
	}
		
	// If the character is currently in a portal, process teleportation mechanics
	if(bIsInPortal && ActivePortalSystem) {
		UpdatePortalMaterial();
		TeleportTick();
	}
}

void ABeatEmUpCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	InGameUI = Cast<UInGameUI>(CreateWidget(GetGameInstance(), InGameUIClass));
	InGameUI->Player = this;
	InGameUI->UpdateValues();
	InGameUI->AddToViewport();

	UMaterialInterface* Material01 = GetMesh()->GetMaterial(0);
	UMaterialInterface* Material02 = GetMesh()->GetMaterial(1);
	PlayerMaterialInstance01 = UMaterialInstanceDynamic::Create(Material01, this);
	PlayerMaterialInstance02 = UMaterialInstanceDynamic::Create(Material02, this);
	GetMesh()->SetMaterial(0, PlayerMaterialInstance01);
	GetMesh()->SetMaterial(1, PlayerMaterialInstance02);

	bIsRewinding = false;
	GetWorld()->GetTimerManager().SetTimer(RecordingTimerHandle, this, &ABeatEmUpCharacter::NotifyRecordAll, RecordingInterval, true);
}

//////////////////////////////////////////////////////////////////////////
// Punch
void ABeatEmUpCharacter::Punch() {
	// Early exit if punch is not ready
	if(!bPunchReady) return;

	bPunchReady = false;
	GetWorld()->GetTimerManager().SetTimer(PunchTimerHandle, this, &ABeatEmUpCharacter::ResetPunch, PunchCooldown, false);

	if(PunchEffectClass) {
		UNiagaraComponent* SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PunchEffectClass, GetActorLocation(), GetActorRotation());
		SpawnEffect->SetColorParameter(FName("User.SpawnColour"), FLinearColor::MakeRandomColor());
	}

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
	InGameUI->UpdateValues();
}

void ABeatEmUpCharacter::Use() {
	FVector Start = GetFollowCamera()->GetComponentLocation();
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

void ABeatEmUpCharacter::AddExp(int ExpToAdd) {
	CurrentExp += ExpToAdd;
	while(CurrentExp > ExpToLevel) {
		CurrentExp -= ExpToLevel;
		ExpToLevel *= IncreaseMultiplier;
		PunchDamage *= IncreaseMultiplier;
		MaxHealth *= IncreaseMultiplier;
		CurrentHealth = MaxHealth;
	}
	InGameUI->UpdateValues();
}

void ABeatEmUpCharacter::PauseGame() {
	if(UGameplayStatics::IsGamePaused(GetWorld())) return;

	UGameplayStatics::SetGamePaused(GetWorld(), true);
	if(UPauseMenu* PauseMenu = Cast<UPauseMenu>(CreateWidget(GetGameInstance(), PauseMenuClass))) {
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
        GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
        PauseMenu->AddToViewport();
	}
}

void ABeatEmUpCharacter::UpdateGrapplingMaterial() {
	if(!PlayerMaterialInstance01 || !PlayerMaterialInstance02) return;

	PlayerMaterialInstance01->SetScalarParameterValue("Metal_Desaturation", -5);
    PlayerMaterialInstance02->SetScalarParameterValue("Metal_Desaturation", -5);
}

void ABeatEmUpCharacter::UpdatePortalMaterial() {
	if(!PlayerMaterialInstance01 || !PlayerMaterialInstance02) return;
	if(!bIsInPortal) return;
	
	float Frequency = 5.0f;
	const float Brightness = -200 * abs(sin(GetGameTimeSinceCreation() * Frequency));
	FLinearColor InitialTint(0, 0.6f, 1.0f);
	PlayerMaterialInstance01->SetVectorParameterValue("Tint", InitialTint);
	PlayerMaterialInstance01->SetScalarParameterValue("Plastic_Brightness", Brightness);
	PlayerMaterialInstance01->SetScalarParameterValue("Metal_Brightness", Brightness);

	PlayerMaterialInstance02->SetVectorParameterValue("Tint", InitialTint);
	PlayerMaterialInstance02->SetScalarParameterValue("Plastic_Brightness", Brightness);
	PlayerMaterialInstance02->SetScalarParameterValue("Metal_Brightness", Brightness);
}

//////////////////////////////////////////////////////////////////////////
// Rewind
void ABeatEmUpCharacter::RecordState() {
	if(bIsRewinding) return;

	if(RecordedLocations.Num() >= MaxRecordedStates) {
		RecordedLocations.RemoveAt(0);
		RecordedRotations.RemoveAt(0);
		RecordedHealths.RemoveAt(0);
	}

	RecordedLocations.Add(GetActorLocation());
	RecordedRotations.Add(GetActorRotation());
	RecordedHealths.Add(CurrentHealth);
}

void ABeatEmUpCharacter::RewindState() {
	if(!bIsRewinding) return;
	
	if(RecordedLocations.Num() > 0) {
		SetActorLocation(RecordedLocations.Pop());
		SetActorRotation(RecordedRotations.Pop());
		CurrentHealth = RecordedHealths.Pop();
		InGameUI->UpdateValues();
	}
	else
		StopRewind(); 
}

void ABeatEmUpCharacter::NotifyRewindAll() const {
	if(!bIsRewinding) return;
	
	// Notify the game mode to rewind all reversible actors
	if(ABeatEmUpGameMode* GameMode = Cast<ABeatEmUpGameMode>(UGameplayStatics::GetGameMode(this)))
		GameMode->RewindAllReversibleActors();
}

void ABeatEmUpCharacter::NotifyRecordAll() {
	if(bIsRewinding) return;
	
	// Stop rewinding for all reversible actors
	if(ABeatEmUpGameMode* GameMode = Cast<ABeatEmUpGameMode>(UGameplayStatics::GetGameMode(this)))
		GameMode->RecordAllReversibleActors();
}

void ABeatEmUpCharacter::NotifyRewindStatus(bool bRewindingStatus) {
	// Notify the game mode to set the rewind status for all enemies
	if (ABeatEmUpGameMode* GameMode = Cast<ABeatEmUpGameMode>(UGameplayStatics::GetGameMode(this)))
		GameMode->SetRewindStatusForEnemies(bRewindingStatus);
}

void ABeatEmUpCharacter::ActivateRewind() {
	if(bIsRewinding) return;
	if(RecordedLocations.Num() == 0) return;

	bIsRewinding = true;
	GetWorld()->GetTimerManager().SetTimer(RewindTimerHandle, this, &ABeatEmUpCharacter::NotifyRewindAll, RecordingInterval, true);
	NotifyRewindStatus(true);
}

void ABeatEmUpCharacter::StopRewind() {
	if(!bIsRewinding) return;

	bIsRewinding = false;
	GetWorld()->GetTimerManager().ClearTimer(RewindTimerHandle);
	NotifyRewindStatus(false);
}

void ABeatEmUpCharacter::DealDamage(float Damage) {
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	InGameUI->UpdateValues();
	if(CurrentHealth == 0)
		Cast<APlayerController>(GetController())->RestartLevel();
}

//////////////////////////////////////////////////////////////////////////
// Grapple
void ABeatEmUpCharacter::LaunchGrapplingHook() {
	// Check if the grappling hook is already active; if so, deactivate it
	if(bIsGrapplingHookActive) {
		UE_LOG(LogTemp, Warning, TEXT("Already Activated!"));
		StopGrapplingHook();
		return;
	}
	
	FVector Start = GetFollowCamera()->GetComponentLocation();
	FVector End = Start + GetFollowCamera()->GetForwardVector() * GrapplingDistance;

	FHitResult HitData;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECC_Visibility, TraceParams);

	Start = GetActorLocation();
	// Optionally draw a debug line to visualize the grappling hook path
	if(bEnableDebug)
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0, 1.0f);

	if(!bHit) return;
	if(HitData.GetActor() == nullptr) return;

	if(bEnableDebug)
		DrawDebugSphere(GetWorld(), HitData.ImpactPoint, 10.0f, 12, FColor::Red, false, 5.0f, 0, 1.0f);

	FVector Direction = (HitData.ImpactPoint - Start).GetSafeNormal();
	FRotator LaunchRotation = Direction.Rotation();

	// Create the grappling hook at the location of the character, pointing towards the hit point
	if (AGrapplingHook* GrapplingHook = GetWorld()->SpawnActor<AGrapplingHook>(GrapplingHookClass, Start, LaunchRotation)) {
		GrapplingHook->Launch(HitData.ImpactPoint, this);
		LaunchedGrapplingHook = GrapplingHook;
		bIsGrapplingHookActive = true;
	}
}

void ABeatEmUpCharacter::StartGrapplingHook(const FVector& TargetLocation) {
	if(bIsGrappling) return;  // Exit if grappling is already in progress

	GrapplingHookTarget = TargetLocation;
	GrapplingForce = GetCharacterMovement()->Velocity;
	bIsGrappling = true;

	// Set character movement mode to flying to enable aerial maneuvers
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void ABeatEmUpCharacter::StopGrapplingHook() {
	if(!bIsGrappling) return;
	
	PlayerMaterialInstance01->SetScalarParameterValue("Metal_Desaturation", 0);
	PlayerMaterialInstance02->SetScalarParameterValue("Metal_Desaturation", 0);

	bIsGrappling = false;
	bIsGrapplingHookActive = false;
	LaunchedGrapplingHook->Destroy();
	LaunchedGrapplingHook = nullptr;

	// Reset character movement mode to falling after grappling is stopped
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

//////////////////////////////////////////////////////////////////////////
// Grenade

// Sets the character to have a grenade available for throwing
void ABeatEmUpCharacter::PickUpGrenade() {
	bHasGrenade = true;
}

void ABeatEmUpCharacter::ThrowGrenade() {
	if(!bHasGrenade) return;

	bHasGrenade = false;
	FVector ThrowDirection = GetFollowCamera()->GetForwardVector() + FVector::UpVector;
	ThrowDirection.Normalize();
	
	ThrowDirection *= ThrowForce;  // Apply force to the throw direction
	if(AGrenade* Grenade = GetWorld()->SpawnActor<AGrenade>(GrenadeClass, GetActorLocation() + FVector(0, 0, 50), FRotator::ZeroRotator))
		Grenade->Initialize(ThrowDirection, bEnableDebug);  // Initialize the grenade's trajectory
}

//////////////////////////////////////////////////////////////////////////
// Portal
void ABeatEmUpCharacter::StartPortalCreation() {
	if(!bIsPortalReady) {
		UE_LOG(LogTemp, Error, TEXT("Portal not ready!, %s seoconds left."), *FString::SanitizeFloat(GetWorld()->GetTimerManager().GetTimerRemaining(PortalCooldownTimerHandle)));
		return;
	}
	if(bIsInPortal) return;
	
	if(!ActivePortalSystem) {
		ActivePortalSystem = GetWorld()->SpawnActor<APortalSystem>(PortalSystemClass);
		ActivePortalSystem->CreateEntryPortal(GetActorLocation(), GetActorRotation() + FRotator(90.0f, 0.0f, 0.0f));
		SetPortalCooldown();
	}
	else if(ActivePortalSystem->EntryPortal && !ActivePortalSystem->ExitPortal)
		EndPortalCreation();
	else
		UE_LOG(LogTemp, Error, TEXT("Something went wrong!"));
}

void ABeatEmUpCharacter::EndPortalCreation() {
	if(!ActivePortalSystem) return;
	if(!bIsPortalReady) return;
	if(bIsInPortal) return;

	bIsPortalReady = false;
	ActivePortalSystem->CreateExitPortal(GetActorLocation(), GetActorRotation() + FRotator(90.0f, 0.0f, 0.0f));
}

void ABeatEmUpCharacter::EnterPortal(const APortal* Portal) {
	if(!ActivePortalSystem) return;
	if(bIsInPortal) return;
	
	bIsInPortal = true;
	
	TeleportTrailComponent->ActivateSystem();
	// Reverse the teleport direction if entering the exit portal
	if (Portal == Cast<APortal>(ActivePortalSystem->ExitPortal)) {
		CurrentSnapshotIndex = ActivePortalSystem->TransformSnapshots.Num() - 1;
		CurrentSpeedMultiplier = -SpeedMultiplier;
	}
	else {
		CurrentSnapshotIndex = 0;
		CurrentSpeedMultiplier = SpeedMultiplier;
	}

	if(!SpawnedLightActor && TeleportLightActorClass) {
		const FVector LightLocation = GetActorLocation();
		SpawnedLightActor = Cast<ATeleportLight>(GetWorld()->SpawnActor(TeleportLightActorClass, &LightLocation));
		SpawnedLightActor->TotalSnapshots = ActivePortalSystem->TransformSnapshots.Num();
	}
}

void ABeatEmUpCharacter::ExitPortal() {
	if(!ActivePortalSystem) return;
	if(!bIsInPortal) return;

	bIsInPortal = false;
	CurrentSnapshotIndex = 0;
	TeleportTrailComponent->DeactivateSystem();

	if(!PlayerMaterialInstance01 || !PlayerMaterialInstance02) return;
	
	FLinearColor InitialTint(1.0f, 1.0f, 1.0f);
	PlayerMaterialInstance01->SetVectorParameterValue("Tint", InitialTint);
	PlayerMaterialInstance01->SetScalarParameterValue("Plastic_Brightness", 1);
	PlayerMaterialInstance01->SetScalarParameterValue("Metal_Brightness", 1);

	PlayerMaterialInstance02->SetVectorParameterValue("Tint", InitialTint);
	PlayerMaterialInstance02->SetScalarParameterValue("Plastic_Brightness", 1);
	PlayerMaterialInstance02->SetScalarParameterValue("Metal_Brightness", 1);
	
	if(SpawnedLightActor) {
		SpawnedLightActor->Destroy();
		SpawnedLightActor = nullptr;
	}
}

void ABeatEmUpCharacter::SetPortalCooldown() {
	GetWorld()->GetTimerManager().SetTimer(PortalCooldownTimerHandle, this, &ABeatEmUpCharacter::ResetPortal, PortalCooldown, false);
}

void ABeatEmUpCharacter::ResetPortal() {
	bIsPortalReady = true;
}

void ABeatEmUpCharacter::TeleportTick() {
	if ((CurrentSpeedMultiplier > 0 && CurrentSnapshotIndex >= ActivePortalSystem->TransformSnapshots.Num()) ||
		(CurrentSpeedMultiplier < 0 && CurrentSnapshotIndex < 0)) {
		ExitPortal();
		return;
	}

	FTransform Snapshot = ActivePortalSystem->TransformSnapshots[CurrentSnapshotIndex];

	// Adjust rotation if moving backwards through the teleport
	if (CurrentSpeedMultiplier < 0) {
		FRotator NewRotation = Snapshot.GetRotation().Rotator();
		NewRotation.Yaw += 180.0f;
		Snapshot.SetRotation(NewRotation.Quaternion());
	}

	SetActorTransform(Snapshot);
	CurrentSnapshotIndex += CurrentSpeedMultiplier;

	// Update the light properties based on the current snapshot index
	if(SpawnedLightActor)
		SpawnedLightActor->UpdateLightProperties(CurrentSnapshotIndex);
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

		// Throwing Grenade
		EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::PickUpGrenade);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::ThrowGrenade);

		// Creating Portal
		EnhancedInputComponent->BindAction(PortalAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::StartPortalCreation);

		// Pausing the game
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::PauseGame);

		// Rewinding time
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Started, this, &ABeatEmUpCharacter::ActivateRewind);
		EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Completed, this, &ABeatEmUpCharacter::StopRewind);
	}
	else {
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
