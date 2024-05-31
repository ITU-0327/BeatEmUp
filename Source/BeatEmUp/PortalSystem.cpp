// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSystem.h"

#include "BeatEmUpCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortalSystem::APortalSystem() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APortalSystem::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void APortalSystem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Continuously record the player's position before the exit portal is created
	if(EntryPortal && !ExitPortal)
		RecordSnapshot();
}

void APortalSystem::CreateEntryPortal(const FVector& EntryLocation, const FRotator& EntryRotation) {
	const FRotator AdjustedRotation = EntryRotation + FRotator(90.0f, 0.0f, 0.0f);
	EntryPortal = GetWorld()->SpawnActor(PortalClass, &EntryLocation, &AdjustedRotation, FActorSpawnParameters());
	if(!EntryPortal)
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn entry portal"));
}

void APortalSystem::CreateExitPortal(const FVector& ExitLocation, const FRotator& ExitRotation) {
	const FRotator AdjustedRotation = ExitRotation + FRotator(90.0f, 0.0f, 0.0f);
	ExitPortal = GetWorld()->SpawnActor(PortalClass, &ExitLocation, &AdjustedRotation, FActorSpawnParameters());
	if(!ExitPortal) {
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn exit portal"));
		return;
	}
	Cast<APortal>(EntryPortal)->ActivatePortal();
	Cast<APortal>(ExitPortal)->ActivatePortal();

	// Set a timer to automatically end the portal session after a duration
	GetWorld()->GetTimerManager().SetTimer(PortalTimerHandle, this, &APortalSystem::AutoEndPortal, PortalDuration, false);
}

void APortalSystem::RecordSnapshot() {
	if(const ABeatEmUpCharacter* Player = Cast<ABeatEmUpCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))) {
		const FTransform CurrentTransform = Player->GetActorTransform();

		// Check if there are any snapshots recorded
		if(TransformSnapshots.Num() != 0 && CurrentTransform.Equals(TransformSnapshots.Last(), 0.01f)) return;

		TransformSnapshots.Add(CurrentTransform);
	}
}

void APortalSystem::AutoEndPortal() {
	EntryPortal->Destroy();
	ExitPortal->Destroy();

	// Clear the active portal system from the player character
	if(ABeatEmUpCharacter* Player = Cast<ABeatEmUpCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))) {
		Player->ExitPortal();
		Player->ActivePortalSystem = nullptr;
	}
	
	Destroy();
}
