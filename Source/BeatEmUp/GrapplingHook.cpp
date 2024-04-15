// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplingHook.h"

#include "BeatEmUpCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AGrapplingHook::AGrapplingHook() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hook Mesh"));
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	
	CableComponent->SetupAttachment(RootComponent);
	CableComponent->SetVisibility(true);

	CableComponent->CableLength = 1;
	CableComponent->NumSegments = 1;
	CableComponent->CableWidth = 10;
}

void AGrapplingHook::Launch(const FVector& NewTargetLocation, AActor* NewInitiator) {
	TargetLocation = NewTargetLocation;
	Initiator = NewInitiator;
	
	if(Initiator == nullptr) return;
	if(CableComponent == nullptr) return;

	CableComponent->SetAttachEndToComponent(Initiator->GetRootComponent());
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
		ABeatEmUpCharacter* Character = Cast<ABeatEmUpCharacter>(Initiator);
        if(Character == nullptr) return;
        
        if(!Character->bIsGrappling)
        	Character->FlyTowardPoint(TargetLocation);
    
        bool bCutRope = false;
        
        if(FVector::Dist(GetActorLocation(), Initiator->GetActorLocation()) < AttachThreshold)
        	bCutRope = true;

		const FVector CableDirection = (GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
		const FVector CharacterDirection = Character->GetActorRotation().Vector();
        if(FVector::DotProduct(CharacterDirection, CableDirection) <= 0)
        	bCutRope = true;
        
        if(bCutRope) {
        	Character->bIsGrappling = false;
            Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
            Destroy();
        }
	}
}
