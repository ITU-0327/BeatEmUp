// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpGameMode.h"
#include "BeatEmUpCharacter.h"
#include "BeatEmUpGameInstance.h"
#include "BrainComponent.h"
#include "EnemyBTController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABeatEmUpGameMode::ABeatEmUpGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABeatEmUpGameMode::BeginPlay() {
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &ABeatEmUpGameMode::PostBeginPlay);
}

void ABeatEmUpGameMode::Load(UBeatEmUpSaveGame* LoadedGame) {
	ABeatEmUpCharacter* PlayerCharacter = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!PlayerCharacter) return;

	PlayerCharacter->SetActorLocationAndRotation(LoadedGame->PlayerLocation, LoadedGame->PlayerRotation);
	PlayerCharacter->CurrentHealth = LoadedGame->PlayerCurrentHealth;
	PlayerCharacter->MaxHealth = LoadedGame->PlayerMaxHealth;
	PlayerCharacter->PunchDamage = LoadedGame->PlayerPunchDamage;
	PlayerCharacter->CurrentExp = LoadedGame->PlayerCurrentExp;
	PlayerCharacter->ExpToLevel = LoadedGame->PlayerExpToLevel;
	PlayerCharacter->InGameUI->UpdateValues();

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
}

void ABeatEmUpGameMode::Save(UBeatEmUpSaveGame* SaveGame) {
	const ABeatEmUpCharacter* PlayerCharacter = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if(!PlayerCharacter) return;
	
	SaveGame->PlayerLocation = PlayerCharacter->GetActorLocation();
	SaveGame->PlayerRotation = PlayerCharacter->GetActorRotation();
	SaveGame->PlayerCurrentHealth = PlayerCharacter->CurrentHealth;
	SaveGame->PlayerMaxHealth = PlayerCharacter->MaxHealth;
	SaveGame->PlayerPunchDamage = PlayerCharacter->PunchDamage;
	SaveGame->PlayerCurrentExp = PlayerCharacter->CurrentExp;
	SaveGame->PlayerExpToLevel = PlayerCharacter->ExpToLevel;

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
}

void ABeatEmUpGameMode::PostBeginPlay() {
	const UBeatEmUpGameInstance* GameInstance = Cast<UBeatEmUpGameInstance>(GetGameInstance());
	if(!GameInstance) return;
	if(!GameInstance->bLoadSave) return;

	if(UBeatEmUpSaveGame* LoadedGame = Cast<UBeatEmUpSaveGame>(UGameplayStatics::LoadGameFromSlot("TestSave", 0)))
		Load(LoadedGame);
}
