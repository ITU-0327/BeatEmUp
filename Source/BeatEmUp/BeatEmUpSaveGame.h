// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BeatEmUpSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API UBeatEmUpSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
		FString SaveSlotName;
	UPROPERTY()
		uint32 UserIndex;

	// Player data
	UPROPERTY()
		FVector PlayerLocation;
	UPROPERTY()
		FRotator PlayerRotation;
	UPROPERTY()
		float PlayerCurrentHealth;
	UPROPERTY()
		float PlayerMaxHealth;
	UPROPERTY()
		float PlayerCurrentExp;
	UPROPERTY()
		float PlayerExpToLevel;
	UPROPERTY()
		float PlayerPunchDamage;

	// Enemy data
	UPROPERTY()
		TArray<FVector> EnemyLocations;
	UPROPERTY()
		TArray<FRotator> EnemyRotations;
	UPROPERTY()
		TArray<float> EnemyCurrentHealths;
	UPROPERTY()
		TArray<float> EnemyMaxHealths;
	UPROPERTY()
		TArray<bool> EnemyRagdollStates;
	UPROPERTY()
		TArray<FVector> EnemyMeshLocations;
	UPROPERTY()
		TArray<FVector> EnemyMeshVelocities;

	// Grappling hook data
	UPROPERTY()
		FVector GrapplingHookLocation;
	UPROPERTY()
		FRotator GrapplingHookRotation;
	UPROPERTY()
		bool bIsGrapplingHookActive;
	UPROPERTY()
		FVector GrapplingHookTargetLocation;
	UPROPERTY()
		AActor* GrapplingHookTargetActor;
	UPROPERTY()
		UPrimitiveComponent* GrapplingHookTargetComponent;
};
