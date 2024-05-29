// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingHook.h"

#include "BeatEmUpCharacter.h"
#include "Enemy.h"

// Sets default values
AGrapplingHook::AGrapplingHook() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and configure the hook mesh with collision properties
	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook Mesh"));
	HookMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HookMesh->SetCollisionResponseToAllChannels(ECR_Block);
	HookMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	HookMesh->OnComponentHit.AddDynamic(this, &AGrapplingHook::OnHit);
	RootComponent = HookMesh;
	
	// Set up the cable component for visual representation of the grappling hook cable
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	CableComponent->SetupAttachment(RootComponent);
	CableComponent->SetVisibility(true);
	CableComponent->CableLength = 1;
	CableComponent->NumSegments = 1;
	CableComponent->CableWidth = 2;

	// Configure the projectile movement for the hook
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = InitFlingSpeed;
	ProjectileMovement->MaxSpeed = MaxFlyingSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AGrapplingHook::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AGrapplingHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector HookLocation = GetActorLocation();
	ABeatEmUpCharacter* Character = Cast<ABeatEmUpCharacter>(Initiator);

	if(!Character) return;

	const float DistanceToTarget = FVector::Dist(HookLocation, TargetLocation);
	const float DistanceToCharacter = FVector::Dist(HookLocation, Character->GetActorLocation());

	// Determine if the hook should start retracting
	if(!bRetracting) {
		if(DistanceToTarget < AttachThreshold)
			bRetracting = true;
		return;
	}
    
    if(!Character->bIsGrappling)
        Character->StartGrapplingHook(TargetLocation);
    
	// Stop grappling if the character is too close or facing away from the hook
	if(DistanceToCharacter < AttachThreshold ||
    	FVector::DotProduct(Character->GetActorForwardVector(), (HookLocation - Character->GetActorLocation()).GetSafeNormal()) < 0) {
    	Character->StopGrapplingHook();
    	return;
    }
	
	// Apply pulling force to the target if grappling
	if(Character->bIsGrappling) {
		float PullForce;
		// Adjust pulling force based on whether the target is an enemy or a physics object
		if (TargetComponent && TargetComponent->IsSimulatingPhysics())
			PullForce = 70000.f / TargetComponent->GetMass();
		else if (Cast<AEnemy>(TargetActor))
			PullForce = 1000.f;
		else
			return;

		const FVector PullDirection = (Character->GetActorLocation() - HookLocation).GetSafeNormal();
		const FVector TargetPull = PullDirection * PullForce * DeltaTime;
		TargetActor->SetActorLocation(TargetActor->GetActorLocation() + TargetPull);
	}
}

void AGrapplingHook::Launch(const FVector& NewTargetLocation, AActor* NewInitiator) {
	TargetLocation = NewTargetLocation;
	Initiator = NewInitiator;
	
	if(Initiator == nullptr) return;
	if(CableComponent == nullptr) return;

	CableComponent->SetAttachEndToComponent(Initiator->GetRootComponent());

	const FVector LaunchDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
	ProjectileMovement->Velocity = LaunchDirection * ProjectileMovement->InitialSpeed;
}

void AGrapplingHook::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit) {
	if(OtherActor == Initiator) return;

	TargetLocation = Hit.ImpactPoint;
	
	if (Cast<AEnemy>(OtherActor) || (OtherComp && OtherComp->IsSimulatingPhysics()))
		AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
	
	TargetActor = OtherActor;
	TargetComponent = OtherComp;
}
