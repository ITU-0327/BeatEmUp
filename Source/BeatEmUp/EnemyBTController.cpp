// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBTController.h"
#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

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

	if(const ABeatEmUpCharacter* PlayerCharacter = Cast<ABeatEmUpCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		bDrawDebug = PlayerCharacter->bEnableDebug;

	NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());

	UseBlackboard(AIBlackboard, BlackboardComponent);
	RunBehaviorTree(BehaviorTree);

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBTController::OnSensesUpdated);
	BlackboardComponent->SetValueAsBool("HasAmmo", true);
}

void AEnemyBTController::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	DrawDebugVision();

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

		if(const AEnemy* Enemy = Cast<AEnemy>(GetPawn()))
			Enemy->HealthBar->ShowWarningIcon();

		AlertNearbyEnemies();
	}
	else {
		TargetPlayer = nullptr;
		BlackboardComponent->SetValueAsBool("ChasePlayer", false);

		if(const AEnemy* Enemy = Cast<AEnemy>(GetPawn()))
			Enemy->HealthBar->HideWarningIcon();
	}
}

void AEnemyBTController::Shoot() {
	const FVector ShootDirection = TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation();
	Cast<AEnemy>(GetPawn())->Shoot(ShootDirection);
	Ammo--;
	BlackboardComponent->SetValueAsBool("HasAmmo", Ammo > 0);
}

void AEnemyBTController::AlertNearbyEnemies() {
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetPawn());

	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		GetPawn()->GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(AlertRadius),
		QueryParams
	);

	for(FOverlapResult Result : OverlapResults) {
		APawn* EnemyPawn = Cast<APawn>(Result.GetActor());
		if(!EnemyPawn) continue;
		if(EnemyPawn == GetPawn()) continue;
		
		if (AEnemyBTController* EnemyController = Cast<AEnemyBTController>(EnemyPawn->GetController()))
			EnemyController->OnAlerted(TargetPlayer);
	}
}

void AEnemyBTController::OnAlerted(APawn* AlertedPlayer) {
	TargetPlayer = AlertedPlayer;
	BlackboardComponent->SetValueAsBool("ChasePlayer", true);
	BlackboardComponent->SetValueAsVector("PlayerLocation", TargetPlayer->GetActorLocation());

	if(const AEnemy* Enemy = Cast<AEnemy>(GetPawn()))
		Enemy->HealthBar->ShowWarningIcon();
}

void AEnemyBTController::DrawDebugVision() const {
	if(!bDrawDebug) return;
	if (!GetPawn()) return;

	FVector Start = GetPawn()->GetActorLocation();
	float HalfAngleRadians = FMath::DegreesToRadians(FieldOfView / 2);

	FVector Forward = GetPawn()->GetActorForwardVector();

	DrawDebugCone(
		GetWorld(),
		Start,
		Forward,
		SightRadius,
		HalfAngleRadians,
		HalfAngleRadians,
		32,
		FColor::Green,
		false,
		-1,
		0,
		1
	);
}
