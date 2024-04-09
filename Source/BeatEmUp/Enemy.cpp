// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;	
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	GetCharacterMovement()->bOrientRotationToMovement = true;  // Always rotates to face the direction that it's moving in.
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Player == nullptr) return;

	const FVector TowardsPlayer = Player->GetActorLocation() - GetActorLocation();
	AddMovementInput(TowardsPlayer);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::DealDamage(float Damage) {
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}

void AEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal,
	const FHitResult& Hit) {
	if(OtherActor == Player) Destroy();
}

