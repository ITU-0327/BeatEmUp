// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Enemy.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	MovementComponent->bShouldBounce = true;
	MovementComponent->BounceVelocityStopSimulatingThreshold = MovementSpeed / 2;
	MovementComponent->bSweepCollision = true;
	MovementComponent->InitialSpeed = MovementSpeed;
	MovementComponent->UpdatedComponent = Mesh;
	MovementComponent->ProjectileGravityScale = 0;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay() {
	Super::BeginPlay();
	Mesh->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	Mesh->SetNotifyRigidBodyCollision(true);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector Normal,
	const FHitResult& Hit) {
	if(!OtherActor) return;
	if(OtherActor == this) return;

	if(AEnemy* HitEnemy = Cast<AEnemy>(OtherActor)) {
		HitEnemy->DealDamage(Damage);
		Destroy();
	}

	if(ABeatEmUpCharacter* HitCharacter = Cast<ABeatEmUpCharacter>(OtherActor)) {
		HitCharacter->DealDamage(Damage);
		Destroy();
	}
}
