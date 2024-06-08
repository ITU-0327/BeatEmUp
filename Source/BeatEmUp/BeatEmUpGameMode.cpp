// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpGameMode.h"
#include "BeatEmUpCharacter.h"
#include "BeatEmUpGameInstance.h"
#include "EnemyBTController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABeatEmUpGameMode::ABeatEmUpGameMode() {
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if(PlayerPawnBPClass.Class)
		DefaultPawnClass = PlayerPawnBPClass.Class;
}

void ABeatEmUpGameMode::BeginPlay() {
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &ABeatEmUpGameMode::PostBeginPlay);
}

void ABeatEmUpGameMode::RecordAllReversibleActors() const {
	TArray<AActor*> ReversibleActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UReversible::StaticClass(), ReversibleActors);

	for(AActor* Actor : ReversibleActors)
		if(IReversible* ReversibleActor = Cast<IReversible>(Actor))
			ReversibleActor->RecordState();
}

void ABeatEmUpGameMode::RewindAllReversibleActors() const {
	TArray<AActor*> ReversibleActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UReversible::StaticClass(), ReversibleActors);

	for(AActor* Actor : ReversibleActors)
		if(IReversible* ReversibleActor = Cast<IReversible>(Actor))
			ReversibleActor->RewindState();
}

void ABeatEmUpGameMode::SetRewindStatusForEnemies(bool bIsRewinding) {
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);

	for(AActor* Enemy : Enemies) {
		if(AEnemy* EnemyCharacter = Cast<AEnemy>(Enemy)) {
			if (AEnemyBTController* EnemyController = Cast<AEnemyBTController>(EnemyCharacter->GetController()))
				EnemyController->SetIsRewinding(bIsRewinding);
		}
	}
}

void ABeatEmUpGameMode::Load(UBeatEmUpSaveGame* LoadedGame) const {
	// Player data
	ABeatEmUpCharacter* PlayerCharacter = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!PlayerCharacter) return;

	PlayerCharacter->SetActorLocationAndRotation(LoadedGame->PlayerLocation, LoadedGame->PlayerRotation);
	PlayerCharacter->CurrentHealth = LoadedGame->PlayerCurrentHealth;
	PlayerCharacter->MaxHealth = LoadedGame->PlayerMaxHealth;
	PlayerCharacter->PunchDamage = LoadedGame->PlayerPunchDamage;
	PlayerCharacter->CurrentExp = LoadedGame->PlayerCurrentExp;
	PlayerCharacter->ExpToLevel = LoadedGame->PlayerExpToLevel;
	PlayerCharacter->InGameUI->UpdateValues();

	// Enemy data
	for(int i = 0; i < LoadedGame->EnemyLocations.Num(); i++) {
		AEnemy* SpawnedEnemy = Cast<AEnemy>(GetWorld()->SpawnActor(EnemyClass));
		if (!SpawnedEnemy) continue;
		
		SpawnedEnemy->SetActorLocationAndRotation(LoadedGame->EnemyLocations[i], LoadedGame->EnemyRotations[i]);
		SpawnedEnemy->CurrentHealth = LoadedGame->EnemyCurrentHealths[i];
		SpawnedEnemy->MaxHealth = LoadedGame->EnemyMaxHealths[i];
		if(LoadedGame->EnemyRagdollStates[i]) {
			SpawnedEnemy->Ragdoll();
			SpawnedEnemy->GetCharacterMovement()->GravityScale = 0;
			SpawnedEnemy->GetMesh()->SetWorldLocation(LoadedGame->EnemyMeshLocations[i], false, nullptr, ETeleportType::TeleportPhysics);
			SpawnedEnemy->GetMesh()->SetAllPhysicsLinearVelocity(LoadedGame->EnemyMeshVelocities[i], true);
		}
		else {
			SpawnedEnemy->Ragdoll();
			SpawnedEnemy->StopRagdoll();
		}
	}

	// Grappling hook data
	if(LoadedGame->GrapplingHookLocation != FVector::ZeroVector) {
		AGrapplingHook* GrapplingHook = GetWorld()->SpawnActor<AGrapplingHook>(PlayerCharacter->GrapplingHookClass, LoadedGame->GrapplingHookLocation, LoadedGame->GrapplingHookRotation);
        PlayerCharacter->LaunchedGrapplingHook = GrapplingHook;
        GrapplingHook->Initiator = PlayerCharacter;
        
        GrapplingHook->TargetLocation = LoadedGame->GrapplingHookTargetLocation;
        if(LoadedGame->bIsGrapplingHookActive)
        	PlayerCharacter->StartGrapplingHook(LoadedGame->GrapplingHookTargetLocation);
    
        GrapplingHook->TargetActor = LoadedGame->GrapplingHookTargetActor;
        GrapplingHook->TargetComponent = LoadedGame->GrapplingHookTargetComponent;
    
        if(PlayerCharacter->GetRootComponent() && GrapplingHook->CableComponent)
        	GrapplingHook->CableComponent->SetAttachEndToComponent(PlayerCharacter->GetRootComponent());
	}
	

	// Portal data
	if(LoadedGame->EntryPortalLocation != FVector::ZeroVector) {
		APortalSystem* PortalSystem = GetWorld()->SpawnActor<APortalSystem>(PlayerCharacter->PortalSystemClass);
		PortalSystem->CreateEntryPortal(LoadedGame->EntryPortalLocation, LoadedGame->EntryPortalRotation);
		if(LoadedGame->ExitPortalLocation != FVector::ZeroVector)
			PortalSystem->CreateExitPortal(LoadedGame->ExitPortalLocation, LoadedGame->ExitPortalRotation);
		PortalSystem->TransformSnapshots = LoadedGame->TransformSnapshots;
		PlayerCharacter->ActivePortalSystem = PortalSystem;
	}
}

void ABeatEmUpGameMode::Save(UBeatEmUpSaveGame* SaveGame) const {
	// Player data
	const ABeatEmUpCharacter* PlayerCharacter = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if(!PlayerCharacter) return;
	
	SaveGame->PlayerLocation = PlayerCharacter->GetActorLocation();
	SaveGame->PlayerRotation = PlayerCharacter->GetActorRotation();
	SaveGame->PlayerCurrentHealth = PlayerCharacter->CurrentHealth;
	SaveGame->PlayerMaxHealth = PlayerCharacter->MaxHealth;
	SaveGame->PlayerPunchDamage = PlayerCharacter->PunchDamage;
	SaveGame->PlayerCurrentExp = PlayerCharacter->CurrentExp;
	SaveGame->PlayerExpToLevel = PlayerCharacter->ExpToLevel;

	// Enemy data
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);
	for (AActor* Enemy : Enemies) {
		AEnemy* EnemyCharacter = Cast<AEnemy>(Enemy);
		if (!EnemyCharacter) continue;

		SaveGame->EnemyLocations.Add(EnemyCharacter->GetActorLocation());
		SaveGame->EnemyRotations.Add(EnemyCharacter->GetActorRotation());
		SaveGame->EnemyCurrentHealths.Add(EnemyCharacter->CurrentHealth);
		SaveGame->EnemyMaxHealths.Add(EnemyCharacter->MaxHealth);
		bool bIsRagdoll = EnemyCharacter->GetMesh()->GetCollisionProfileName() == "Ragdoll";
		SaveGame->EnemyRagdollStates.Add(bIsRagdoll);
		SaveGame->EnemyMeshLocations.Add(EnemyCharacter->GetMesh()->GetComponentLocation());
		SaveGame->EnemyMeshVelocities.Add(EnemyCharacter->GetMesh()->GetComponentVelocity());
	}

	// Grappling hook data
	if(const AGrapplingHook* GrapplingHook = PlayerCharacter->LaunchedGrapplingHook) {
		SaveGame->GrapplingHookLocation = GrapplingHook->GetActorLocation();
        SaveGame->GrapplingHookRotation = GrapplingHook->GetActorRotation();
        SaveGame->bIsGrapplingHookActive = GrapplingHook->bIsPulling || GrapplingHook->bRetracting;
        SaveGame->GrapplingHookTargetLocation = GrapplingHook->TargetLocation;
        if(GrapplingHook->TargetActor)
        	SaveGame->GrapplingHookTargetActor = GrapplingHook->TargetActor;
        if(GrapplingHook->TargetComponent)
        	SaveGame->GrapplingHookTargetComponent = GrapplingHook->TargetComponent;
	}
	
	// Portal data
	if(const APortalSystem* PortalSystem = PlayerCharacter->ActivePortalSystem) {
		if (PortalSystem->EntryPortal) {
			SaveGame->EntryPortalLocation = PortalSystem->EntryPortal->GetActorLocation();
			SaveGame->EntryPortalRotation = PortalSystem->EntryPortal->GetActorRotation();
		}
		if (PortalSystem->ExitPortal) {
			SaveGame->ExitPortalLocation = PortalSystem->ExitPortal->GetActorLocation();
			SaveGame->ExitPortalRotation = PortalSystem->ExitPortal->GetActorRotation();
		}
		SaveGame->TransformSnapshots = PortalSystem->TransformSnapshots;
	}
}

void ABeatEmUpGameMode::PostBeginPlay() {
	const UBeatEmUpGameInstance* GameInstance = Cast<UBeatEmUpGameInstance>(GetGameInstance());
	if(!GameInstance) return;
	if(!GameInstance->bLoadSave) return;

	if(UBeatEmUpSaveGame* LoadedGame = Cast<UBeatEmUpSaveGame>(UGameplayStatics::LoadGameFromSlot("TestSave", 0)))
		Load(LoadedGame);
}
