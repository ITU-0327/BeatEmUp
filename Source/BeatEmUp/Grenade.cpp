// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"

#include "BeatEmUpCharacter.h"
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
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Explosion, this, &AGrenade::Explode, TimeToExplode, false);
}

// Called every frame
void AGrenade::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AGrenade::Initialize(const FVector& ThrowDirection, const bool bDebug) {
	Mesh->SetSimulatePhysics(true);
	
	Mesh->AddImpulse(ThrowDirection);
	bEnableDebug = bDebug;
}

void AGrenade::Explode() {
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, true);
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

	TArray<AActor*> IgnoredActors;
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.0f, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	if(bEnableDebug)
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 32, FColor::Red, false, 5.0f);

	Destroy();
}
