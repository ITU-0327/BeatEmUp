// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingHook.h"

// Sets default values
AGrapplingHook::AGrapplingHook() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook Mesh"));
	RootComponent = HookMesh;
}

void AGrapplingHook::Launch(const FVector& NewTargetLocation, AActor* NewInitiator) {
	TargetLocation = NewTargetLocation;
	Initiator = NewInitiator;
}

// Called when the game starts or when spawned
void AGrapplingHook::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AGrapplingHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bRetracting) {
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, FlyingSpeed));
		if(FVector::Dist(GetActorLocation(), TargetLocation) < AttachThreshold)
			bRetracting = true;
	}
	else {
		FVector Direction = (Initiator->GetActorLocation() - GetActorLocation()).GetSafeNormal() * FlyingSpeed;
		SetActorLocation(GetActorLocation() + Direction * DeltaTime);
		if(FVector::Dist(GetActorLocation(), Initiator->GetActorLocation()) < AttachThreshold)
			Destroy();
	}
}
