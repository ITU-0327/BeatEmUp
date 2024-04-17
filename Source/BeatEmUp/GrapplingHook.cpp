// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingHook.h"

#include "BeatEmUpCharacter.h"
#include "Enemy.h"
#include "Components/SphereComponent.h"

// Sets default values
AGrapplingHook::AGrapplingHook() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook Mesh"));
	HookMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HookMesh->SetCollisionResponseToAllChannels(ECR_Block);
	HookMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	HookMesh->OnComponentHit.AddDynamic(this, &AGrapplingHook::OnHit);
	RootComponent = HookMesh;
	
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	CableComponent->SetupAttachment(RootComponent);
	CableComponent->SetVisibility(true);
	CableComponent->CableLength = 1;
	CableComponent->NumSegments = 1;
	CableComponent->CableWidth = 2;

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

	if(!bRetracting) {
		// SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetLocation, DeltaTime, MaxFlyingSpeed));
		if(FVector::Dist(GetActorLocation(), TargetLocation) < AttachThreshold)
			bRetracting = true;
	}
	else {
		ABeatEmUpCharacter* Character = Cast<ABeatEmUpCharacter>(Initiator);
        if(Character == nullptr) return;
        
        if(!Character->bIsGrappling)
        	Character->StartGrapplingHook(TargetLocation);
    
        bool bCutRope = false;
        
        if(FVector::Dist(GetActorLocation(), Initiator->GetActorLocation()) < AttachThreshold)
        	bCutRope = true;

		const FVector CableDirection = (GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
		const FVector CharacterDirection = Character->GetActorRotation().Vector();
        if(FVector::DotProduct(CharacterDirection, CableDirection) <= 0)
        	bCutRope = true;
        
        if(bCutRope) {
        	Character->StopGrapplingHook();
            Destroy();
        }
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
	
	if (AEnemy* HitEnemy = Cast<AEnemy>(OtherActor)) {
		UE_LOG(LogTemp, Warning, TEXT("OnHit() Called by %s"), *OtherActor->GetName());
		AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
	}
	else if (OtherComp && OtherComp->IsSimulatingPhysics()) {
		AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
	}
	
}
