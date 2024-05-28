// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicSpotlight.h"

#include "Enemy.h"

// Sets default values
ADynamicSpotlight::ADynamicSpotlight() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	RootComponent = SpotLight;
}

// Called when the game starts or when spawned
void ADynamicSpotlight::BeginPlay() {
	Super::BeginPlay();

	if(bStartingSpotlight)
		TargetActor = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Called every frame
void ADynamicSpotlight::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(!TargetActor) return;

	FVector TargetDirection = TargetActor->GetActorLocation() - GetActorLocation();
	SetActorRotation(TargetDirection.Rotation());
	if(!bStartingSpotlight) {
		if(const AEnemy* Enemy = Cast<AEnemy>(TargetActor)) {
			SetActorLocation(Enemy->GetMesh()->GetComponentLocation() + ActorOffset);
			TargetDirection = Enemy->GetMesh()->GetComponentLocation() - GetActorLocation();
			SetActorRotation(TargetDirection.Rotation());
			SpotLight->SetLightColor(FLinearColor::LerpUsingHSV(HealthyColour, DeadColour, 1 - (float)Enemy->CurrentHealth / Enemy->MaxHealth));
		}
		else
			SetActorLocation(TargetActor->GetActorLocation() + ActorOffset);
	}
	else {
		if(const ABeatEmUpCharacter* Character = Cast<ABeatEmUpCharacter>(TargetActor))
			SpotLight->SetLightColor(FLinearColor::LerpUsingHSV(HealthyColour, DeadColour, 1 - (float)Character->CurrentHealth / Character->MaxHealth));
	}
}

