// Fill out your copyright notice in the Description page of Project Settings.


#include "WreckingBall.h"

// Sets default values
AWreckingBall::AWreckingBall() {
	PrimaryActorTick.bCanEverTick = true;

	TopAnchor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopAnchor"));
	RootComponent = TopAnchor;
	TopAnchor->SetSimulatePhysics(false);

	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	Cable->SetupAttachment(TopAnchor);
	Cable->CableLength = 300;
	Cable->NumSegments = 20;
	Cable->SolverIterations = 5;
	Cable->bEnableCollision = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	BallMesh->SetupAttachment(Cable);
	BallMesh->SetSimulatePhysics(true);
	BallMesh->SetMassOverrideInKg(NAME_None, 5000, true);

	Cable->EndLocation = FVector(0, 0, 0);

	ConstraintComp = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("ConstraintComp"));
	ConstraintComp->SetupAttachment(RootComponent);

	// Attach top anchor and ball to the constraint
	ConstraintComp->ConstraintActor1 = this;  // The wrecking ball actor itself
	ConstraintComp->ComponentName1.ComponentName = TEXT("TopAnchor");
	ConstraintComp->ConstraintActor2 = this;  // Also this actor
	ConstraintComp->ComponentName2.ComponentName = TEXT("BallMesh");

	// Configure the constraint to limit movement exactly to the cable length
	ConstraintComp->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 300);
	ConstraintComp->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0);
	ConstraintComp->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0);

	ConstraintComp->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0);
	ConstraintComp->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0);
	ConstraintComp->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0);
}

// Called when the game starts or when spawned
void AWreckingBall::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AWreckingBall::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
