// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

#include "BeatEmUpCharacter.h"

// Sets default values
APortal::APortal() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PortalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PortalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PortalMesh->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);

	RootComponent = PortalMesh;
}

// Called when the game starts or when spawned
void APortal::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void APortal::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if(!bIsPortalActive) return;
	if(ABeatEmUpCharacter* Player = Cast<ABeatEmUpCharacter>(OtherActor))
		Player->EnterPortal(this);
	UE_LOG(LogTemp, Warning, TEXT("Portal(%s) Overlap with %s"), *GetName(), *OtherActor->GetName());
}
