// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBTController.h"

AEnemyBTController::AEnemyBTController() {
	PrimaryActorTick.bCanEverTick = true;
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = FieldOfView;
	SightConfig->SetMaxAge(SightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AAIController::GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	AAIController::GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AEnemyBTController::BeginPlay() {
	Super::BeginPlay();

	NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());

	UseBlackboard(AIBlackboard, BlackboardComponent);
	RunBehaviorTree(BehaviorTree);

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBTController::OnSensesUpdated);
}

void AEnemyBTController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if(!TargetPlayer) return;

	BlackboardComponent->SetValueAsVector("PlayerLocation", TargetPlayer->GetActorLocation());
}

FRotator AEnemyBTController::GetControlRotation() const {
	if(GetPawn())
		return FRotator(0, GetPawn()->GetActorRotation().Yaw, 0);
	
	return Super::GetControlRotation();
}

void AEnemyBTController::GenerateNewRandomLocation() {
	if(!NavSystem) return;
	
	FNavLocation ReturnLocation;
	NavSystem->GetRandomReachablePointInRadius(GetPawn()->GetActorLocation(), PatrolDistance, ReturnLocation);
	BlackboardComponent->SetValueAsVector("PatrolPoint", ReturnLocation.Location);
}

void AEnemyBTController::OnSensesUpdated(AActor* UpdatedActor, FAIStimulus Stimulus) {
	APawn* SensedPawn = Cast<APawn>(UpdatedActor);
	if(!SensedPawn) return;
	if(!SensedPawn->IsPlayerControlled()) return;

	if(Stimulus.WasSuccessfullySensed()) {
		TargetPlayer = SensedPawn;
		BlackboardComponent->SetValueAsBool("ChasePlayer", true);
		BlackboardComponent->SetValueAsVector("PlayerLocation", TargetPlayer->GetActorLocation());
	}
	else {
		TargetPlayer = nullptr;
		BlackboardComponent->SetValueAsBool("ChasePlayer", false);
	}
}
