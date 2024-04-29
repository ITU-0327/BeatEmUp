// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy.h"
#include "NavigationSystem.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UENUM()
enum EEnemyState
{
	IDLE,
	PATROL,
	CHASE,
};

UCLASS()
class BEATEMUP_API AEnemyController : public AAIController
{
	GENERATED_BODY()

	AEnemyController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual FRotator GetControlRotation() const override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	EEnemyState CurrentState = IDLE;

	void ChangeState(EEnemyState NewState);

	void OnIdleEnter();
	void OnIdleTick(float DeltaTime);
	void OnIdleExit();

	void OnPatrolEnter();
	void OnPatrolTick(float DeltaTime);
	void OnPatrolExit();

	void OnChaseEnter();
	void OnChaseTick(float DeltaTime);
	void OnChaseExit();

	float TimeInState = 0;
	UPROPERTY(EditAnywhere)
		float DetectionRange = 1000;
	UPROPERTY(EditAnywhere)
		float PatrolRange = 2000;
	UPROPERTY()
		UNavigationSystemV1* NavigationSystem;
	UPROPERTY()
		AEnemy* PossessedEnemy;
	FNavLocation PatrolTarget;
};
