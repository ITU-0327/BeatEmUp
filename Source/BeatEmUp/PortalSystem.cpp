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
	EntryPortal = GetWorld()->SpawnActor(PortalClass, &EntryLocation, &EntryRotation, FActorSpawnParameters());
	if(!EntryPortal)
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn entry portal"));
}

void APortalSystem::CreateExitPortal(const FVector& ExitLocation, const FRotator& ExitRotation) {
	ExitPortal = GetWorld()->SpawnActor(PortalClass, &ExitLocation, &ExitRotation, FActorSpawnParameters());
	if(!ExitPortal) {
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn exit portal"));
		return;
	}
	Cast<APortal>(EntryPortal)->bIsPortalActive = true;
	Cast<APortal>(ExitPortal)->bIsPortalActive = true;

	// Set a timer to automatically end the portal session after a duration
	GetWorld()->GetTimerManager().SetTimer(PortalTimerHandle, this, &APortalSystem::AutoEndPortal, PortalDuration, false);
}

void APortalSystem::RecordSnapshot() {
	if(const ABeatEmUpCharacter* Player = Cast<ABeatEmUpCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		TransformSnapshots.Add(FTransformSnapshot(Player->GetActorTransform()));
}

void APortalSystem::AutoEndPortal() {
	EntryPortal->Destroy();
	ExitPortal->Destroy();

	// Clear the active portal system from the player character
	if(ABeatEmUpCharacter* Player = Cast<ABeatEmUpCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		Player->ActivePortalSystem = nullptr;
	Destroy();
}
