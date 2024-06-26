// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyBTController.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API AEnemyBTController : public AAIController
{
	GENERATED_BODY()

	AEnemyBTController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual FRotator GetControlRotation() const override;
	
public:
	void GenerateNewRandomLocation() const;

	UFUNCTION()
		void OnSensesUpdated(AActor* UpdatedActor, FAIStimulus Stimulus);

	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float SightRadius = 600;
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float SightAge = 5;
	UPROPERTY(EditAnywhere)
		float LoseSightRadius = SightRadius + 300;
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float FieldOfView = 45;
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float PatrolDistance = 2000;
	UPROPERTY(EditAnywhere)
		UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditAnywhere)
		UBlackboardData* AIBlackboard;
	UPROPERTY(EditAnywhere)
		UBehaviorTree* BehaviorTree;
	UPROPERTY()
		UBlackboardComponent* BlackboardComponent;
	UPROPERTY()
		UNavigationSystemV1* NavSystem;
	UPROPERTY()
		APawn* TargetPlayer;

	// Shooting
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float Ammo = 5;
	void Shoot();

	// Alert
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		float AlertRadius = 1000;
	void AlertNearbyEnemies() const;
	void OnAlerted(const FVector& LastSeenLocation) const;
	void ShowWarningIcon() const;
	void HideWarningIcon() const;
	void ShowInvestigatingIcon() const;
	void HideInvestigatingIcon() const;
	void SetIsRewinding(bool bIsRewinding);

	// Debug
	UPROPERTY(EditAnywhere, Category="Enemy Settings")
		bool bDrawDebug = true;
	void DrawDebugVision() const;
};
