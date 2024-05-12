// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"

#include "BeatEmUpCharacter.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGrenade::AGrenade() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay() {
	Super::BeginPlay();
	
	// Set a timer to trigger the explosion after a predefined delay
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Explosion, this, &AGrenade::Explode, TimeToExplode, false);
}

// Called every frame
void AGrenade::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AGrenade::Initialize(const FVector& ThrowDirection, const bool bDebug) {
	Mesh->SetSimulatePhysics(true);  // Enable physics simulation for the grenade
	
	Mesh->AddImpulse(ThrowDirection);  // Apply an initial force in the specified direction
	bEnableDebug = bDebug; // Store the debug state
}

void AGrenade::Explode() {
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, true);
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);  // Ensure the grenade does not damage itself
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.0f, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	TArray<FHitResult> HitResults;
	FCollisionShape ExplosionSphere = FCollisionShape::MakeSphere(ExplosionRadius);
	GetWorld()->SweepMultiByChannel(HitResults, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_PhysicsBody, ExplosionSphere);

	TArray<AActor*> HitActors;
	for (FHitResult Hit : HitResults) {
		if(HitActors.Contains(Hit.GetActor())) continue;

		HitActors.Add(Hit.GetActor());
		UMeshComponent* HitMesh;
		if(AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor())) {
			HitMesh = Cast<UMeshComponent>(HitEnemy->GetMesh());
			HitEnemy->Ragdoll();
			HitEnemy->DealDamage(ExplosionDamage);
		}
		else {
			HitMesh = Cast<UMeshComponent>(Hit.GetActor()->GetRootComponent());
		}

		if(HitMesh == nullptr) continue;

		HitMesh->AddRadialImpulse(GetActorLocation(), ExplosionRadius, ExplosionForce, RIF_Linear, true);
	}
	
	if(bEnableDebug)
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 32, FColor::Red, false, 5.0f);

	Destroy();
}
